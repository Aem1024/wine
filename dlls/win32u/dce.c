/*
 * Window painting functions
 *
 * Copyright 1993, 1994, 1995, 2001, 2004, 2005, 2008 Alexandre Julliard
 * Copyright 1996, 1997, 1999 Alex Korobka
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

#if 0
#pragma makedep unix
#endif

#include <assert.h>
#include <pthread.h>
#include "ntstatus.h"
#define WIN32_NO_STATUS
#include "ntgdi_private.h"
#include "ntuser_private.h"
#include "wine/server.h"
#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(win);

static struct list dce_list = LIST_INIT(dce_list);

#define DCE_CACHE_SIZE 64

static struct list window_surfaces = LIST_INIT( window_surfaces );
static pthread_mutex_t surfaces_lock = PTHREAD_MUTEX_INITIALIZER;

/*******************************************************************
 * Dummy window surface for windows that shouldn't get painted.
 */

static void CDECL dummy_surface_lock( struct window_surface *window_surface )
{
    /* nothing to do */
}

static void CDECL dummy_surface_unlock( struct window_surface *window_surface )
{
    /* nothing to do */
}

static void *CDECL dummy_surface_get_bitmap_info( struct window_surface *window_surface, BITMAPINFO *info )
{
    static DWORD dummy_data;

    info->bmiHeader.biSize          = sizeof( info->bmiHeader );
    info->bmiHeader.biWidth         = dummy_surface.rect.right;
    info->bmiHeader.biHeight        = dummy_surface.rect.bottom;
    info->bmiHeader.biPlanes        = 1;
    info->bmiHeader.biBitCount      = 32;
    info->bmiHeader.biCompression   = BI_RGB;
    info->bmiHeader.biSizeImage     = 0;
    info->bmiHeader.biXPelsPerMeter = 0;
    info->bmiHeader.biYPelsPerMeter = 0;
    info->bmiHeader.biClrUsed       = 0;
    info->bmiHeader.biClrImportant  = 0;
    return &dummy_data;
}

static RECT *CDECL dummy_surface_get_bounds( struct window_surface *window_surface )
{
    static RECT dummy_bounds;
    return &dummy_bounds;
}

static void CDECL dummy_surface_set_region( struct window_surface *window_surface, HRGN region )
{
    /* nothing to do */
}

static void CDECL dummy_surface_flush( struct window_surface *window_surface )
{
    /* nothing to do */
}

static void CDECL dummy_surface_destroy( struct window_surface *window_surface )
{
    /* nothing to do */
}

static const struct window_surface_funcs dummy_surface_funcs =
{
    dummy_surface_lock,
    dummy_surface_unlock,
    dummy_surface_get_bitmap_info,
    dummy_surface_get_bounds,
    dummy_surface_set_region,
    dummy_surface_flush,
    dummy_surface_destroy
};

struct window_surface dummy_surface = { &dummy_surface_funcs, { NULL, NULL }, 1, { 0, 0, 1, 1 } };

/*******************************************************************
 *           register_window_surface
 *
 * Register a window surface in the global list, possibly replacing another one.
 */
void register_window_surface( struct window_surface *old, struct window_surface *new )
{
    if (old == &dummy_surface) old = NULL;
    if (new == &dummy_surface) new = NULL;
    if (old == new) return;
    pthread_mutex_lock( &surfaces_lock );
    if (old) list_remove( &old->entry );
    if (new) list_add_tail( &window_surfaces, &new->entry );
    pthread_mutex_unlock( &surfaces_lock );
}

/*******************************************************************
 *           flush_window_surfaces
 *
 * Flush pending output from all window surfaces.
 */
void flush_window_surfaces( BOOL idle )
{
    static DWORD last_idle;
    DWORD now;
    struct window_surface *surface;

    pthread_mutex_lock( &surfaces_lock );
    now = NtGetTickCount();
    if (idle) last_idle = now;
    /* if not idle, we only flush if there's evidence that the app never goes idle */
    else if ((int)(now - last_idle) < 50) goto done;

    LIST_FOR_EACH_ENTRY( surface, &window_surfaces, struct window_surface, entry )
        surface->funcs->flush( surface );
done:
    pthread_mutex_unlock( &surfaces_lock );
}

/***********************************************************************
 *           update_visible_region
 *
 * Set the visible region and X11 drawable for the DC associated to
 * a given window.
 */
static void update_visible_region( struct dce *dce )
{
    struct window_surface *surface = NULL;
    NTSTATUS status;
    HRGN vis_rgn = 0;
    HWND top_win = 0;
    DWORD flags = dce->flags;
    DWORD paint_flags = 0;
    size_t size = 256;
    RECT win_rect, top_rect;
    WND *win;

    /* don't clip siblings if using parent clip region */
    if (flags & DCX_PARENTCLIP) flags &= ~DCX_CLIPSIBLINGS;

    /* fetch the visible region from the server */
    do
    {
        RGNDATA *data = malloc( sizeof(*data) + size - 1 );
        if (!data) return;

        SERVER_START_REQ( get_visible_region )
        {
            req->window  = wine_server_user_handle( dce->hwnd );
            req->flags   = flags;
            wine_server_set_reply( req, data->Buffer, size );
            if (!(status = wine_server_call( req )))
            {
                size_t reply_size = wine_server_reply_size( reply );
                data->rdh.dwSize   = sizeof(data->rdh);
                data->rdh.iType    = RDH_RECTANGLES;
                data->rdh.nCount   = reply_size / sizeof(RECT);
                data->rdh.nRgnSize = reply_size;
                vis_rgn = NtGdiExtCreateRegion( NULL, data->rdh.dwSize + data->rdh.nRgnSize, data );

                top_win         = wine_server_ptr_handle( reply->top_win );
                win_rect.left   = reply->win_rect.left;
                win_rect.top    = reply->win_rect.top;
                win_rect.right  = reply->win_rect.right;
                win_rect.bottom = reply->win_rect.bottom;
                top_rect.left   = reply->top_rect.left;
                top_rect.top    = reply->top_rect.top;
                top_rect.right  = reply->top_rect.right;
                top_rect.bottom = reply->top_rect.bottom;
                paint_flags     = reply->paint_flags;
            }
            else size = reply->total_size;
        }
        SERVER_END_REQ;
        free( data );
    } while (status == STATUS_BUFFER_OVERFLOW);

    if (status || !vis_rgn) return;

    user_driver->pGetDC( dce->hdc, dce->hwnd, top_win, &win_rect, &top_rect, flags );

    if (dce->clip_rgn) NtGdiCombineRgn( vis_rgn, vis_rgn, dce->clip_rgn,
                                        (flags & DCX_INTERSECTRGN) ? RGN_AND : RGN_DIFF );

    /* don't use a surface to paint the client area of OpenGL windows */
    if (!(paint_flags & SET_WINPOS_PIXEL_FORMAT) || (flags & DCX_WINDOW))
    {
        win = get_win_ptr( top_win );
        if (win && win != WND_DESKTOP && win != WND_OTHER_PROCESS)
        {
            surface = win->surface;
            if (surface) window_surface_add_ref( surface );
            release_win_ptr( win );
        }
    }

    if (!surface) SetRectEmpty( &top_rect );
    __wine_set_visible_region( dce->hdc, vis_rgn, &win_rect, &top_rect, surface );
    if (surface) window_surface_release( surface );
}

/***********************************************************************
 *           release_dce
 */
static void release_dce( struct dce *dce )
{
    if (!dce->hwnd) return;  /* already released */

    __wine_set_visible_region( dce->hdc, 0, &dummy_surface.rect, &dummy_surface.rect, &dummy_surface );
    user_driver->pReleaseDC( dce->hwnd, dce->hdc );

    if (dce->clip_rgn) NtGdiDeleteObjectApp( dce->clip_rgn );
    dce->clip_rgn = 0;
    dce->hwnd     = 0;
    dce->flags   &= DCX_CACHE;
}

/***********************************************************************
 *           delete_clip_rgn
 */
static void delete_clip_rgn( struct dce *dce )
{
    if (!dce->clip_rgn) return;  /* nothing to do */

    dce->flags &= ~(DCX_EXCLUDERGN | DCX_INTERSECTRGN);
    NtGdiDeleteObjectApp( dce->clip_rgn );
    dce->clip_rgn = 0;

    /* make it dirty so that the vis rgn gets recomputed next time */
    SetHookFlags( dce->hdc, DCHF_INVALIDATEVISRGN );
}

/***********************************************************************
 *           dc_hook
 *
 * See "Undoc. Windows" for hints (DC, SetDCHook, SetHookFlags)..
 */
static BOOL CALLBACK dc_hook( HDC hDC, WORD code, DWORD_PTR data, LPARAM lParam )
{
    BOOL retv = TRUE;
    struct dce *dce = (struct dce *)data;

    TRACE("hDC = %p, %u\n", hDC, code);

    if (!dce) return FALSE;
    assert( dce->hdc == hDC );

    switch( code )
    {
    case DCHC_INVALIDVISRGN:
        /* GDI code calls this when it detects that the
         * DC is dirty (usually after SetHookFlags()). This
         * means that we have to recompute the visible region.
         */
        if (dce->count) update_visible_region( dce );
        else /* non-fatal but shouldn't happen */
            WARN("DC is not in use!\n");
        break;
    case DCHC_DELETEDC:
        user_lock();
        if (!(dce->flags & DCX_CACHE))
        {
            WARN("Application trying to delete an owned DC %p\n", dce->hdc);
            retv = FALSE;
        }
        else
        {
            list_remove( &dce->entry );
            if (dce->clip_rgn) NtGdiDeleteObjectApp( dce->clip_rgn );
            free( dce );
        }
        user_unlock();
        break;
    }
    return retv;
}

/***********************************************************************
 *           alloc_dce
 *
 * Allocate a new DCE.
 */
static struct dce *alloc_dce(void)
{
    struct dce *dce;

    if (!(dce = malloc( sizeof(*dce) ))) return NULL;
    if (!(dce->hdc = NtGdiOpenDCW( NULL, NULL, NULL, 0, TRUE, 0, NULL, NULL )))
    {
        free( dce );
        return 0;
    }
    dce->hwnd      = 0;
    dce->clip_rgn  = 0;
    dce->flags     = 0;
    dce->count     = 1;

    /* store DCE handle in DC hook data field */
    SetDCHook( dce->hdc, dc_hook, (DWORD_PTR)dce );
    SetHookFlags( dce->hdc, DCHF_INVALIDATEVISRGN );
    return dce;
}

/***********************************************************************
 *           get_window_dce
 */
static struct dce *get_window_dce( HWND hwnd )
{
    struct dce *dce;
    WND *win = get_win_ptr( hwnd );

    if (!win || win == WND_OTHER_PROCESS || win == WND_DESKTOP) return NULL;

    dce = win->dce;
    if (!dce && (dce = get_class_dce( win->class )))
    {
        win->dce = dce;
        dce->count++;
    }
    release_win_ptr( win );

    if (!dce)  /* try to allocate one */
    {
        struct dce *dce_to_free = NULL;
        LONG class_style = get_class_long( hwnd, GCL_STYLE, FALSE );

        if (class_style & CS_CLASSDC)
        {
            if (!(dce = alloc_dce())) return NULL;

            win = get_win_ptr( hwnd );
            if (win && win != WND_OTHER_PROCESS && win != WND_DESKTOP)
            {
                if (win->dce)  /* another thread beat us to it */
                {
                    dce_to_free = dce;
                    dce = win->dce;
                }
                else if ((win->dce = set_class_dce( win->class, dce )) != dce)
                {
                    dce_to_free = dce;
                    dce = win->dce;
                    dce->count++;
                }
                else
                {
                    dce->count++;
                    list_add_tail( &dce_list, &dce->entry );
                }
                release_win_ptr( win );
            }
            else dce_to_free = dce;
        }
        else if (class_style & CS_OWNDC)
        {
            if (!(dce = alloc_dce())) return NULL;

            win = get_win_ptr( hwnd );
            if (win && win != WND_OTHER_PROCESS && win != WND_DESKTOP)
            {
                if (win->dwStyle & WS_CLIPCHILDREN) dce->flags |= DCX_CLIPCHILDREN;
                if (win->dwStyle & WS_CLIPSIBLINGS) dce->flags |= DCX_CLIPSIBLINGS;
                if (win->dce)  /* another thread beat us to it */
                {
                    dce_to_free = dce;
                    dce = win->dce;
                }
                else
                {
                    win->dce = dce;
                    dce->hwnd = hwnd;
                    list_add_tail( &dce_list, &dce->entry );
                }
                release_win_ptr( win );
            }
            else dce_to_free = dce;
        }

        if (dce_to_free)
        {
            SetDCHook( dce_to_free->hdc, NULL, 0 );
            NtGdiDeleteObjectApp( dce_to_free->hdc );
            free( dce_to_free );
            if (dce_to_free == dce)
                dce = NULL;
        }
    }
    return dce;
}

/***********************************************************************
 *           free_dce
 *
 * Free a class or window DCE.
 */
void free_dce( struct dce *dce, HWND hwnd )
{
    struct dce *dce_to_free = NULL;

    user_lock();

    if (dce)
    {
        if (!--dce->count)
        {
            release_dce( dce );
            list_remove( &dce->entry );
            dce_to_free = dce;
        }
        else if (dce->hwnd == hwnd)
        {
            release_dce( dce );
        }
    }

    /* now check for cache DCEs */

    if (hwnd)
    {
        LIST_FOR_EACH_ENTRY( dce, &dce_list, struct dce, entry )
        {
            if (dce->hwnd != hwnd) continue;
            if (!(dce->flags & DCX_CACHE)) break;

            release_dce( dce );
            if (dce->count)
            {
                WARN( "GetDC() without ReleaseDC() for window %p\n", hwnd );
                dce->count = 0;
                SetHookFlags( dce->hdc, DCHF_DISABLEDC );
            }
        }
    }

    user_unlock();

    if (dce_to_free)
    {
        SetDCHook( dce_to_free->hdc, NULL, 0 );
        NtGdiDeleteObjectApp( dce_to_free->hdc );
        free( dce_to_free );
    }
}

/***********************************************************************
 *           make_dc_dirty
 *
 * Mark the associated DC as dirty to force a refresh of the visible region
 */
static void make_dc_dirty( struct dce *dce )
{
    if (!dce->count)
    {
        /* Don't bother with visible regions of unused DCEs */
        TRACE("purged %p hwnd %p\n", dce->hdc, dce->hwnd);
        release_dce( dce );
    }
    else
    {
        /* Set dirty bits in the hDC and DCE structs */
        TRACE("fixed up %p hwnd %p\n", dce->hdc, dce->hwnd);
        SetHookFlags( dce->hdc, DCHF_INVALIDATEVISRGN );
    }
}

/***********************************************************************
 *           invalidate_dce
 *
 * It is called from SetWindowPos() - we have to
 * mark as dirty all busy DCEs for windows that have pWnd->parent as
 * an ancestor and whose client rect intersects with specified update
 * rectangle. In addition, pWnd->parent DCEs may need to be updated if
 * DCX_CLIPCHILDREN flag is set.
 */
void invalidate_dce( WND *win, const RECT *extra_rect )
{
    DPI_AWARENESS_CONTEXT context;
    RECT window_rect;
    struct dce *dce;

    if (!win->parent) return;

    context = set_thread_dpi_awareness_context( get_window_dpi_awareness_context( win->obj.handle ));
    get_window_rects( win->obj.handle, COORDS_SCREEN, &window_rect, NULL, get_thread_dpi() );

    TRACE("%p parent %p %s (%s)\n",
          win->obj.handle, win->parent, wine_dbgstr_rect(&window_rect), wine_dbgstr_rect(extra_rect) );

    /* walk all DCEs and fixup non-empty entries */

    LIST_FOR_EACH_ENTRY( dce, &dce_list, struct dce, entry )
    {
        if (!dce->hwnd) continue;

        TRACE( "%p: hwnd %p dcx %08x %s %s\n", dce->hdc, dce->hwnd, dce->flags,
               (dce->flags & DCX_CACHE) ? "Cache" : "Owned", dce->count ? "InUse" : "" );

        if ((dce->hwnd == win->parent) && !(dce->flags & DCX_CLIPCHILDREN))
            continue;  /* child window positions don't bother us */

        /* if DCE window is a child of hwnd, it has to be invalidated */
        if (dce->hwnd == win->obj.handle || is_child( win->obj.handle, dce->hwnd ))
        {
            make_dc_dirty( dce );
            continue;
        }

        /* otherwise check if the window rectangle intersects this DCE window */
        if (win->parent == dce->hwnd || is_child( win->parent, dce->hwnd ))
        {
            RECT dce_rect, tmp;
            get_window_rects( dce->hwnd, COORDS_SCREEN, &dce_rect, NULL, get_thread_dpi() );
            if (intersect_rect( &tmp, &dce_rect, &window_rect ) ||
                (extra_rect && intersect_rect( &tmp, &dce_rect, extra_rect )))
                make_dc_dirty( dce );
        }
    }
    set_thread_dpi_awareness_context( context );
}

/***********************************************************************
 *           NtUserGetDCEx (win32u.@)
 */
HDC WINAPI NtUserGetDCEx( HWND hwnd, HRGN clip_rgn, DWORD flags )
{
    const DWORD clip_flags = DCX_PARENTCLIP | DCX_CLIPSIBLINGS | DCX_CLIPCHILDREN | DCX_WINDOW;
    const DWORD user_flags = clip_flags | DCX_NORESETATTRS; /* flags that can be set by user */
    BOOL update_vis_rgn = TRUE;
    struct dce *dce;
    HWND parent;
    LONG window_style = get_window_long( hwnd, GWL_STYLE );

    if (!hwnd) hwnd = get_desktop_window();
    else hwnd = get_full_window_handle( hwnd );

    TRACE( "hwnd %p, clip_rgn %p, flags %08x\n", hwnd, clip_rgn, flags );

    if (!is_window(hwnd)) return 0;

    /* fixup flags */

    if (flags & (DCX_WINDOW | DCX_PARENTCLIP)) flags |= DCX_CACHE;

    if (flags & DCX_USESTYLE)
    {
        flags &= ~(DCX_CLIPCHILDREN | DCX_CLIPSIBLINGS | DCX_PARENTCLIP);

        if (window_style & WS_CLIPSIBLINGS) flags |= DCX_CLIPSIBLINGS;

        if (!(flags & DCX_WINDOW))
        {
            if (get_class_long( hwnd, GCL_STYLE, FALSE ) & CS_PARENTDC) flags |= DCX_PARENTCLIP;

            if (window_style & WS_CLIPCHILDREN && !(window_style & WS_MINIMIZE))
                flags |= DCX_CLIPCHILDREN;
        }
    }

    if (flags & DCX_WINDOW) flags &= ~DCX_CLIPCHILDREN;

    parent = NtUserGetAncestor( hwnd, GA_PARENT );
    if (!parent || (parent == get_desktop_window()))
        flags = (flags & ~DCX_PARENTCLIP) | DCX_CLIPSIBLINGS;

    /* it seems parent clip is ignored when clipping siblings or children */
    if (flags & (DCX_CLIPSIBLINGS | DCX_CLIPCHILDREN)) flags &= ~DCX_PARENTCLIP;

    if( flags & DCX_PARENTCLIP )
    {
        LONG parent_style = get_window_long( parent, GWL_STYLE );
        if( (window_style & WS_VISIBLE) && (parent_style & WS_VISIBLE) )
        {
            flags &= ~DCX_CLIPCHILDREN;
            if (parent_style & WS_CLIPSIBLINGS) flags |= DCX_CLIPSIBLINGS;
        }
    }

    /* find a suitable DCE */

    if ((flags & DCX_CACHE) || !(dce = get_window_dce( hwnd )))
    {
        struct dce *dceEmpty = NULL, *dceUnused = NULL, *found = NULL;
        unsigned int count = 0;

        /* Strategy: First, we attempt to find a non-empty but unused DCE with
         * compatible flags. Next, we look for an empty entry. If the cache is
         * full we have to purge one of the unused entries.
         */
        user_lock();
        LIST_FOR_EACH_ENTRY( dce, &dce_list, struct dce, entry )
        {
            if (!(dce->flags & DCX_CACHE)) break;
            count++;
            if (dce->count) continue;
            dceUnused = dce;
            if (!dce->hwnd) dceEmpty = dce;
            else if ((dce->hwnd == hwnd) && !((dce->flags ^ flags) & clip_flags))
            {
                TRACE( "found valid %p hwnd %p, flags %08x\n", dce->hdc, hwnd, dce->flags );
                found = dce;
                update_vis_rgn = FALSE;
                break;
            }
        }
        if (!found) found = dceEmpty;
        if (!found && count >= DCE_CACHE_SIZE) found = dceUnused;

        dce = found;
        if (dce)
        {
            dce->count = 1;
            SetHookFlags( dce->hdc, DCHF_ENABLEDC );
        }
        user_unlock();

        /* if there's no dce empty or unused, allocate a new one */
        if (!dce)
        {
            if (!(dce = alloc_dce())) return 0;
            dce->flags = DCX_CACHE;
            user_lock();
            list_add_head( &dce_list, &dce->entry );
            user_unlock();
        }
    }
    else
    {
        flags |= DCX_NORESETATTRS;
        if (dce->hwnd != hwnd)
        {
            /* we should free dce->clip_rgn here, but Windows apparently doesn't */
            dce->flags &= ~(DCX_EXCLUDERGN | DCX_INTERSECTRGN);
            dce->clip_rgn = 0;
        }
        else update_vis_rgn = FALSE; /* updated automatically, via DCHook() */
    }

    if (flags & (DCX_INTERSECTRGN | DCX_EXCLUDERGN))
    {
        /* if the extra clip region has changed, get rid of the old one */
        if (dce->clip_rgn != clip_rgn || ((flags ^ dce->flags) & (DCX_INTERSECTRGN | DCX_EXCLUDERGN)))
            delete_clip_rgn( dce );
        dce->clip_rgn = clip_rgn;
        if (!dce->clip_rgn) dce->clip_rgn = NtGdiCreateRectRgn( 0, 0, 0, 0 );
        dce->flags |= flags & (DCX_INTERSECTRGN | DCX_EXCLUDERGN);
        update_vis_rgn = TRUE;
    }

    if (get_window_long( hwnd, GWL_EXSTYLE ) & WS_EX_LAYOUTRTL)
        NtGdiSetLayout( dce->hdc, -1, LAYOUT_RTL );

    dce->hwnd = hwnd;
    dce->flags = (dce->flags & ~user_flags) | (flags & user_flags);

    /* cross-process invalidation is not supported yet, so always update the vis rgn */
    if (!is_current_process_window( hwnd )) update_vis_rgn = TRUE;

    if (SetHookFlags( dce->hdc, DCHF_VALIDATEVISRGN )) update_vis_rgn = TRUE;  /* DC was dirty */

    if (update_vis_rgn) update_visible_region( dce );

    TRACE( "(%p,%p,0x%x): returning %p%s\n", hwnd, clip_rgn, flags, dce->hdc,
           update_vis_rgn ? " (updated)" : "" );
    return dce->hdc;
}

/**********************************************************************
 *           NtUserWindowFromDC (win32u.@)
 */
HWND WINAPI NtUserWindowFromDC( HDC hdc )
{
    struct dce *dce;
    HWND hwnd = 0;

    user_lock();
    dce = (struct dce *)GetDCHook( hdc, NULL );
    if (dce) hwnd = dce->hwnd;
    user_unlock();
    return hwnd;
}
