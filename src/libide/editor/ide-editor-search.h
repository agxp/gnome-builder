/* ide-editor-search.h
 *
 * Copyright (C) 2017 Christian Hergert <chergert@redhat.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <gtksourceview/gtksource.h>

G_BEGIN_DECLS

typedef enum
{
  IDE_EDITOR_SEARCH_NEXT,
  IDE_EDITOR_SEARCH_PREVIOUS,
  IDE_EDITOR_SEARCH_FORWARD,
  IDE_EDITOR_SEARCH_BACKWARD,
} IdeEditorSearchDirection;

#define IDE_TYPE_EDITOR_SEARCH (ide_editor_search_get_type())

G_DECLARE_FINAL_TYPE (IdeEditorSearch, ide_editor_search, IDE, EDITOR_SEARCH, GObject)

IdeEditorSearch *ide_editor_search_new                          (GtkSourceView             *view);
void             ide_editor_search_set_case_sensitive           (IdeEditorSearch           *self,
                                                                 gboolean                   case_sensitive);
gboolean         ide_editor_search_get_case_sensitive           (IdeEditorSearch           *self);
gboolean         ide_editor_search_get_reverse                  (IdeEditorSearch           *self);
void             ide_editor_search_set_reverse                  (IdeEditorSearch           *self,
                                                                 gboolean                   reverse);
void             ide_editor_search_set_search_text              (IdeEditorSearch           *self,
                                                                 const gchar               *search_text);
const gchar     *ide_editor_search_get_search_text              (IdeEditorSearch           *self);
gboolean         ide_editor_search_get_search_text_invalid      (IdeEditorSearch           *self,
                                                                 guint                     *invalid_begin,
                                                                 guint                     *invalid_end,
                                                                 GError                   **error);
void             ide_editor_search_set_visible                  (IdeEditorSearch           *self,
                                                                 gboolean                   visible);
gboolean         ide_editor_search_get_visible                  (IdeEditorSearch           *self);
void             ide_editor_search_set_regex_enabled            (IdeEditorSearch           *self,
                                                                 gboolean                   regex_enabled);
gboolean         ide_editor_search_get_regex_enabled            (IdeEditorSearch           *self);
void             ide_editor_search_set_replacement_text         (IdeEditorSearch           *self,
                                                                 const gchar               *replacement_text);
const gchar     *ide_editor_search_get_replacement_text         (IdeEditorSearch           *self);
gboolean         ide_editor_search_get_replacement_text_invalid (IdeEditorSearch           *self,
                                                                 guint                     *invalid_begin,
                                                                 guint                     *invalid_end);
void             ide_editor_search_set_at_word_boundaries       (IdeEditorSearch           *self,
                                                                 gboolean                   at_word_boundaries);
gboolean         ide_editor_search_get_at_word_boundaries       (IdeEditorSearch           *self);
gboolean         ide_editor_search_get_busy                     (IdeEditorSearch           *self);
guint            ide_editor_search_get_match_count              (IdeEditorSearch           *self);
guint            ide_editor_search_get_match_position           (IdeEditorSearch           *self);
void             ide_editor_search_move                         (IdeEditorSearch           *self,
                                                                 IdeEditorSearchDirection   direction);
void             ide_editor_search_replace                      (IdeEditorSearch           *self);
void             ide_editor_search_replace_all                  (IdeEditorSearch           *self);
void             ide_editor_search_begin_interactive            (IdeEditorSearch           *self);
void             ide_editor_search_end_interactive              (IdeEditorSearch           *self);

G_END_DECLS