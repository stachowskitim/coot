/*
 * validation-graphs/sequence-view-widget.hh
 *
 * Copyright 2023 by Global Phasing Ltd.
 * Author: Jakub Smulski
 *
 * This file is part of Coot
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copies of the GNU General Public License and
 * the GNU Lesser General Public License along with this program; if not,
 * write to the Free Software Foundation, Inc., 51 Franklin Street,
 * Fifth Floor, Boston, MA, 02110-1301, USA.
 * See http://www.gnu.org/licenses/
 *
 */
#ifndef SEQUENCE_VIEW_WIDGET_HH
#define SEQUENCE_VIEW_WIDGET_HH

#include <gtk/gtk.h>
#include <mmdb2/mmdb_manager.h>
#include "geometry/residue-and-atom-specs.hh"

#if __cplusplus > 201402L
    // good times
    #include <memory>
#else
    // cope
    #include <memory>
    // Copied from: https://gist.github.com/chinmaygarde/970fd5bbd124754b7d36
    // Thank you kind man
    namespace std {
        template <typename T, typename... Args>
        unique_ptr<T> make_unique(Args&&... args) {
            return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
        }
    }
#endif


class sv3_box_info_t {
public:
   int imol;
   coot::residue_spec_t residue_spec;
   int x_base;
   int y_base;
   sv3_box_info_t(int imol, mmdb::Residue *residue_p, int x_base, int y_base) : imol(imol), residue_spec(coot::residue_spec_t(residue_p)),
                                                                                x_base(x_base), y_base(y_base) {}
};

G_BEGIN_DECLS

#define COOT_SEQUENCE_VIEW_TYPE (coot_sequence_view_get_type ())
G_DECLARE_FINAL_TYPE  (CootSequenceView, coot_sequence_view, COOT, COOT_SEQUENCE_VIEW, GtkWidget)

CootSequenceView *coot_sequence_view_new();

G_END_DECLS

void coot_sequence_view_set_structure(CootSequenceView* self, int imol, mmdb::Manager *mol);


#endif // SEQUENCE_VIEW_WIDGET_HH
