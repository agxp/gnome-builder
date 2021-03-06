/* gbp-documentation-card-view-addin.h
 *
 * Copyright © 2017 Lucie Charvat <luci.charvat@gmail.com>
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

G_BEGIN_DECLS

#define GBP_TYPE_DOCUMENTATION_CARD_VIEW_ADDIN (gbp_documentation_card_view_addin_get_type())

G_DECLARE_FINAL_TYPE (GbpDocumentationCardViewAddin, gbp_documentation_card_view_addin, GBP, DOCUMENTATION_CARD_VIEW_ADDIN, GObject)

G_END_DECLS
