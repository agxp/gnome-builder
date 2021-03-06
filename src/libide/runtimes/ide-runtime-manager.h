/* ide-runtime-manager.h
 *
 * Copyright © 2016 Christian Hergert <chergert@redhat.com>
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

#include "ide-version-macros.h"

#include "ide-object.h"

G_BEGIN_DECLS

#define IDE_TYPE_RUNTIME_MANAGER (ide_runtime_manager_get_type())

G_DECLARE_FINAL_TYPE (IdeRuntimeManager, ide_runtime_manager, IDE, RUNTIME_MANAGER, IdeObject)

void        _ide_runtime_manager_unload              (IdeRuntimeManager    *self) G_GNUC_INTERNAL;
IDE_AVAILABLE_IN_ALL
IdeRuntime *ide_runtime_manager_get_runtime          (IdeRuntimeManager    *self,
                                                      const gchar          *id);
IDE_AVAILABLE_IN_ALL
void        ide_runtime_manager_add                  (IdeRuntimeManager    *self,
                                                      IdeRuntime           *runtime);
IDE_AVAILABLE_IN_ALL
void        ide_runtime_manager_remove               (IdeRuntimeManager    *self,
                                                      IdeRuntime           *runtime);
IDE_AVAILABLE_IN_ALL
void        ide_runtime_manager_ensure_async         (IdeRuntimeManager    *self,
                                                      const gchar          *runtime_id,
                                                      GCancellable         *cancellable,
                                                      GAsyncReadyCallback   callback,
                                                      gpointer              user_data);
IDE_AVAILABLE_IN_ALL
IdeRuntime *ide_runtime_manager_ensure_finish        (IdeRuntimeManager    *self,
                                                      GAsyncResult         *result,
                                                      GError              **error);
IDE_AVAILABLE_IN_3_28
void        ide_runtime_manager_ensure_config_async  (IdeRuntimeManager    *self,
                                                      IdeConfiguration     *configuration,
                                                      GCancellable         *cancellable,
                                                      GAsyncReadyCallback   callback,
                                                      gpointer              user_data);
IDE_AVAILABLE_IN_3_28
IdeRuntime *ide_runtime_manager_ensure_config_finish (IdeRuntimeManager    *self,
                                                      GAsyncResult         *result,
                                                      GError              **error);

G_END_DECLS
