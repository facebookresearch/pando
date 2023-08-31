import argparse
import sysconfig
import shutil
from pathlib import Path

COMPONENTS = (
    "INCLUDES",
    "WINDOWS_IMP_LIBS",
)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("component", choices=COMPONENTS)
    parser.add_argument("dst", type=Path)
    args = parser.parse_args()

    cfg = sysconfig.get_config_vars()

    if args.component == "INCLUDES":
        src = cfg["INCLUDEPY"]
        shutil.copytree(src, args.dst)

    if args.component == "WINDOWS_IMP_LIBS":
        assert args.dst.is_dir()

        lib_dir = Path(cfg["BINDIR"]) / "libs"
        lib = lib_dir / "python{}.lib".format(cfg["VERSION"])
        debug_lib = lib_dir / "python{}_d.lib".format(cfg["VERSION"])

        for src in (lib, debug_lib):
            if src.exists():
                shutil.copy2(src, args.dst)


if __name__ == "__main__":
    main()
