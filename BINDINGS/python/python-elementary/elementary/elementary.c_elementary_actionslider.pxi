# Copyright 2012 Kai Huuhko <kai.huuhko@gmail.com>
#
# This file is part of python-elementary.
#
# python-elementary is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# python-elementary is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with python-elementary.  If not, see <http://www.gnu.org/licenses/>.
#

def _actionslider_callback_conv(long addr):
    cdef const_char_ptr s = <const_char_ptr>addr
    if s == NULL:
        return None
    else:
        return s

cdef public class Actionslider(Object) [object PyElementaryActionslider, type PyElementaryActionslider_Type]:

    """An actionslider is a switcher for 2 or 3 labels with customizable magnet properties.

    The user drags and releases the indicator, to choose a label.

    Labels occupy the following positions.
      1. Left
      2. Right
      3. Center

    Positions can be enabled or disabled.

    Magnets can be set on the above positions.

    When the indicator is released, it will move to its nearest "enabled and magnetized" position.

    @note: By default all positions are set as enabled.

    This widget emits the following signals, besides the ones sent from @ref Layout:
      - "selected" - when user selects an enabled position (the label is passed as event info)".
      - "pos_changed" - when the indicator reaches any of the positions("left", "right" or "center").

    Default text parts of the actionslider widget that you can use for are:
      - "indicator" - An indicator label of the actionslider
      - "left" - A left label of the actionslider
      - "right" - A right label of the actionslider
      - "center" - A center label of the actionslider

    """

    def __init__(self, c_evas.Object parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_actionslider_add(parent.obj))

    def selected_label_get(self):
        """Get actionslider selected label.

        @return: The selected label
        """
        return elm_actionslider_selected_label_get(self.obj)

    property selected_label:
        def __get__(self):
            return self.selected_label_get()

    def indicator_pos_set(self, pos):
        """Set actionslider indicator position.

        @param pos: The position of the indicator.
        """
        elm_actionslider_indicator_pos_set(self.obj, pos)

    def indicator_pos_get(self):
        """Get actionslider indicator position.

        @return: The position of the indicator.
        """
        return elm_actionslider_indicator_pos_get(self.obj)

    property indicator_pos:
        def __get__(self):
            return self.indicator_pos_get()
        def __set__(self, pos):
            self.indicator_pos_set(pos)

    def magnet_pos_set(self, pos):
        """Set actionslider magnet position. To make multiple positions magnets @c or
        them together(e.g.: ELM_ACTIONSLIDER_LEFT | ELM_ACTIONSLIDER_RIGHT)

        @param pos: Bit mask indicating the magnet positions.
        """
        elm_actionslider_magnet_pos_set(self.obj, pos)

    def magnet_pos_get(self):
        """Get actionslider magnet position.

        @return: The positions with magnet property.
        """
        return elm_actionslider_magnet_pos_get(self.obj)

    property magnet_pos:
        def __get__(self):
            return self.magnet_pos_get()
        def __set__(self, pos):
            self.magnet_pos_set(pos)

    def enabled_pos_set(self, pos):
        """Set actionslider enabled position. To set multiple positions as enabled @c or
        them together(e.g.: ELM_ACTIONSLIDER_LEFT | ELM_ACTIONSLIDER_RIGHT).

        @note: All the positions are enabled by default.

        @param pos: Bit mask indicating the enabled positions.
        """
        elm_actionslider_enabled_pos_set(self.obj, pos)

    def enabled_pos_get(self):
        """Get actionslider enabled position.

        @return: The enabled positions.
        """
        return elm_actionslider_enabled_pos_get(self.obj)

    property enabled_pos:
        def __get__(self):
            return self.enabled_pos_get()
        def __set__(self, pos):
            self.enabled_pos_set(pos)

    def callback_selected_add(self, func, *args, **kwargs):
        self._callback_add_full("selected", _actionslider_callback_conv, func, *args, **kwargs)

    def callback_selected_del(self, func):
        self._callback_del_full("selected", _actionslider_callback_conv, func)

    def callback_pos_changed_add(self, func, *args, **kwargs):
        self._callback_add_full("pos_changed", _actionslider_callback_conv, func, *args, **kwargs)

    def callback_pos_changed_del(self, func):
        self._callback_del_full("pos_changed", _actionslider_callback_conv, func)

_elm_widget_type_register("actionslider", Actionslider)

cdef extern from "Elementary.h": # hack to force type to be known
    cdef PyTypeObject PyElementaryActionslider_Type # hack to install metaclass
_install_metaclass(&PyElementaryActionslider_Type, ElementaryObjectMeta)
