# Please uncomment and modify if you want to use
# Remove comment when you done.

config = {
    "header": {
        "uuid": "6a00e88b-b70a-431c-89c1-e8e655a53c48",
        "type": "SAMSUNG",
        "vendor": "SLSI",
        "product": "factory",
        "variant": "factory",
        "name": "factory",
    },
    "build": {
        "path": "samsung/slsi/factory",
        "file": "samsung_factory.py",
        "location": [
            {
                "src": "src/*.c src/*.h src/Makefile:update src/Kconfig:update",
                "dst": "drivers/samsung/factory/",
            },
       ],
    },
    "features": [
        {
            "label": "Samsung Legacy-Style Relocated GPIO Interface for Factory Mode",
            "type": "boolean",
            "help": "Enable relocated GPIO interface",
            "configs": {
                "True": ["CONFIG_SEC_RELOC_GPIO=m"],
                "False": ["# CONFIG_SEC_RELOC_GPIO is not set"],
            },
            "value": True,
        },
    ],
}


def load():
    return config

if __name__ == '__main__':
    print(load())
