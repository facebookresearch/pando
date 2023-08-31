class GuiConfig(object):
    """Wraps Pando config and adds handling for GUI-only and deprecated config items."""

    # gui_items contains keys and default values for gui-only configuration
    gui_items = {
        "auto_stop": False,
        "experiment_length": 60,
        "channel_rows": 32,
        "channel_columns": 32,
    }

    # deprecated_items contains keys for obsolete config items that may still
    # show up in old config files. They should not be saved, but they should also
    # not cause exceptions on loading
    deprecated_items = [
        "laser_wavelength_nm",
        "optional_device_polling_period_ms",
        "optional_devices",
        "pm400_average_window_ms",
    ]

    def __init__(self, wrapped_config):
        self._wrapped_config = wrapped_config
        self._gui_items = dict(GuiConfig.gui_items)

    def __getattr__(self, key, default=None):
        if key in GuiConfig.gui_items:
            return self._gui_items[key]
        elif key in ["_wrapped_config", "_gui_items"]:
            return self.__dict__[key]
        if key in GuiConfig.deprecated_items:
            return default
        return getattr(self._wrapped_config, key)

    def __dir__(self):
        return list(GuiConfig.gui_items.keys()) + dir(self._wrapped_config)

    def __setattr__(self, key, value):
        if key in GuiConfig.gui_items:
            self._gui_items[key] = value
        elif key in ["_wrapped_config", "_gui_items"]:
            self.__dict__[key] = value
        elif key not in GuiConfig.deprecated_items:
            setattr(self._wrapped_config, key, value)

    def get_wrapped_config(self):
        return self._wrapped_config

    @staticmethod
    def make_factory(config_type):
        def create_config():
            return GuiConfig(config_type())

        return create_config
