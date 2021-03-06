/* ide-test.h
 *
 * Copyright © 2017 Christian Hergert <chergert@redhat.com>
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

#include <glib-object.h>

#include "ide-version-macros.h"

G_BEGIN_DECLS

#define IDE_TYPE_TEST (ide_test_get_type())

typedef enum
{
  IDE_TEST_STATUS_NONE,
  IDE_TEST_STATUS_RUNNING,
  IDE_TEST_STATUS_SUCCESS,
  IDE_TEST_STATUS_FAILED,
} IdeTestStatus;

G_DECLARE_DERIVABLE_TYPE (IdeTest, ide_test, IDE, TEST, GObject)

struct _IdeTestClass
{
  GObjectClass parent;

  /*< private >*/
  gpointer _reserved1;
  gpointer _reserved2;
  gpointer _reserved3;
  gpointer _reserved4;
  gpointer _reserved5;
  gpointer _reserved6;
  gpointer _reserved7;
  gpointer _reserved8;
};

IDE_AVAILABLE_IN_3_28
IdeTest       *ide_test_new               (void);
IDE_AVAILABLE_IN_3_28
const gchar   *ide_test_get_display_name  (IdeTest       *self);
IDE_AVAILABLE_IN_3_28
void           ide_test_set_display_name  (IdeTest       *self,
                                           const gchar   *display_name);
IDE_AVAILABLE_IN_3_28
const gchar   *ide_test_get_group         (IdeTest       *self);
IDE_AVAILABLE_IN_3_28
void           ide_test_set_group         (IdeTest       *self,
                                           const gchar   *group);
IDE_AVAILABLE_IN_3_28
const gchar   *ide_test_get_icon_name     (IdeTest       *self);
IDE_AVAILABLE_IN_3_28
const gchar   *ide_test_get_id            (IdeTest       *self);
IDE_AVAILABLE_IN_3_28
void           ide_test_set_id            (IdeTest       *self,
                                           const gchar   *id);
IDE_AVAILABLE_IN_3_28
IdeTestStatus  ide_test_get_status        (IdeTest       *self);
IDE_AVAILABLE_IN_3_28
void           ide_test_set_status        (IdeTest       *self,
                                           IdeTestStatus  status);

G_END_DECLS
