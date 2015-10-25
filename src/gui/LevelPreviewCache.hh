/*
 * Copyright (C) 2002,2003,2004,2005,2006 Daniel Heck
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */
#ifndef GUI_LEVELPREVIEWCACHE_HH_INCLUDED
#define GUI_LEVELPREVIEWCACHE_HH_INCLUDED

#include "lev/Index.hh"
#include "lev/Proxy.hh"
#include "resource_cache.hh"
#include "video.hh"

#include <map>

namespace enigma { namespace gui {

    /**
     * A singleton manager for level previews with autocaching.
     * Clients just need to know the level proxy to request a preview.
     * <p>
     * Previews will be loaded from levelpack bundles if provided. These
     * preview files have the same name as the levels themself but the 
     * suffix ".png". They just sit aside the levels in the same directory
     * or in the same zip archive. Only local stored previews will be looked
     * for.<p>
     * If no bundled preview exists the preview will be loaded from prior
     * generated and saved instances. Previews will be looked up at the 
     * resourceFS with play time generated previews stored at the userImagePath
     * and installation time generated previews at the system path. The previews
     * will be stored at "data/thumbs" with a subpath that reflectes the level
     * subpath with critical characters replaced for url and other special levels.
     * The level release number is attached to the previewname as "#n" to allow
     * different previews for different releases to exist in parallel.<p>
     * 
     * If no stored preview exists a new one will be generated by loading the
     * level. The preview will be stored to the userImagePath for future use.<p>
     * 
     * All loaded previews will be autocached. Futher requests will be served
     * by the cache. The cache will be autocleared when a change in the
     * current Index is detected.<p>
     * 
     * TODO remove unused preview on filesystem
     * TODO autogenerate previews on install
     */
    class LevelPreviewCache {
    public:
        static LevelPreviewCache *instance();
        static void makeSystemPreview(lev::Proxy *levelProxy,
                const ThumbnailInfo &thumbinfo,
                const std::string &systemDataPath);
        ~LevelPreviewCache();

        ecl::Surface *getPreview (lev::Proxy *levelProxy,
                const ThumbnailInfo &thumbinfo,
            bool allowGeneration, bool &didGenerate);
        ecl::Surface *updatePreview (lev::Proxy *levelProxy, const ThumbnailInfo &thumbinfo);
        void clear();
    protected:
        LevelPreviewCache();
    private:
        static LevelPreviewCache *theSingleton;
       
        /**
         * A mapping of "levelId+levelRelease" to preview surfaces
         */
        typedef std::map<std::string, ecl::Surface*> PreviewMap;

        // ---------- Internal methods ----------

        static ecl::Surface *newPreview (lev::Proxy *levelProxy, const ThumbnailInfo &thumbinfo);
        std::string makePreviewPath(lev::Proxy *levelProxy, const ThumbnailInfo &thumbinfo);
        void savePreview(lev::Proxy *levelProxy, const ThumbnailInfo &thumbinfo, ecl::Surface *s);

        // ---------- Variables ----------

        PreviewMap         cache;       // a second mapping to avoid searched on the filesystem
        enigma::ImageCache imgCache;    // the owner of the preview surfaces -
                                        // cannot be used as mapping as this cache 
                                        // uses the filepath as index and autoloads
                                        // files on "get"-access                                       
        lev::Index        *cachedIndex; // the index that is currently cached
    };
}} // namespace enigma::gui
#endif
