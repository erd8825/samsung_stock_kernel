# Please uncomment and modify if you want to use

config = {
    "header": {
        # Automatically generated
        "uuid": "85993446-8487-4987-9739-c1800ccaf206",
        "type": "SOFTWARE",
        "vendor": "SLSI BSP",
        "product": "SEC CLASS",
        "variant": "SEC CLASS",
        "name": "SEC CLASS",
    },
    "build": {
        "path": "samsung/slsi/class",  # path for reach this file
        "file": "samsung_class.py",  # file name
        "location": [
            {
                "src": "sec_class.c Makefile:update Kconfig:update",
                "dst": "drivers/samsung/",
            },
            {
                "src": "sec_class.h",
                "dst": "include/linux/",
            },
        ],
    },
    "features": [
        {
            "label": "Build Config",
            "type": "list",
            # Add configs for each selection
            "configs": {
                "built-in": [
                    "CONFIG_DRV_SAMSUNG=y"
                ],
                "module": [
                    "CONFIG_DRV_SAMSUNG=m"
                ],
                "not set": [
                    "# CONFIG_DRV_SAMSUNG is not set"
                ],
            },
            "list_value": [
                "not set",
                "module",
                "built-in",
            ],
            # Add default value, it will be set if user does not select.
            "value": "module",
        },
    ],
}


def load():
    return config


if __name__ == "__main__":
    print(load())
