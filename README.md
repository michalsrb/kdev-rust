# kdev-rust

This is plugin for [KDevelop](https://www.kdevelop.org/) that adds support for [Rust language](https://www.rust-lang.org/).

In future this plugin will probably use [Rust Language Server](https://github.com/rust-lang/rfcs/blob/master/text/1317-ide.md), but at this moment it uses its own tool called [rustc2duchain](https://github.com/michalsrb/rustc2duchain).

# Disclaimer

  * This is early experminent. Expect bugs and severe limitations.
  * This is not an "official" KDevelop plugin. I chose the name `kdev-rust` to follow the pattern of naming KDevelop plugins but there may be different official `kdev-rust` plugin in the future.

# Prerequisites

## rustc2duchain

This plugin uses the [rustc2duchain](https://github.com/michalsrb/rustc2duchain) tool to parse and analyze Rust code. You need to have the tool installed to get proper Rust support. Follow instructions in [rustc2duchain README](https://github.com/michalsrb/rustc2duchain/blob/master/README.md).

## Mime type for text/rust

Your system needs to recognize `*.rs` files as `text/rust` mime type, otherwise KDevelop won't activate this plugin for them. On linux you need your [shared-mime-info](https://freedesktop.org/wiki/Software/shared-mime-info) package to be at least version 1.5.

## Build dependencies

You need cmake and a c++ compiler. You need Qt5, KDE5 and KDevPlatform development packages. If you are missing some dependency, cmake will tell you.

# Build and installation

## From source

This plugin uses regular cmake build. You can build and install for example using these commands:

```
git clone https://github.com/michalsrb/kdev-rust.git
cd kdev-rust

mkdir build
cd build

cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr
make

make install
```

You should see this file installed to your system (the exact path may vary):
```
/usr/lib64/qt5/plugins/kdevplatform/24/kdevrustlanguagesupport.so
```

# Configuration

In KDevelop navigate to *Settings -> Configure KDevelop -> Plugins* and enable *Rust Support* plugin.

If you installed the `rustc2duchain` tool into your rustup's nightly bin directory the plugin should work out of the box. Otherwise you have to configure paths to it:

In KDevelop, navigate to *Settings -> Configure KDevelop -> Rust Language Support*.

  * Set path to rustc2duchain binary. For example:

    ```
    /home/michal/projects/rustc2duchain/target/release/rustc2duchain
    ```

  * Set path to directory with rust libraries. This is the directory that contains files like `librustc-<hash>.so` and `libstd-<hash>.rlib`. For example:

    ```
    /home/michal/.multirust/toolchains/nightly-x86_64-unknown-linux-gnu/lib/rustlib/x86_64-unknown-linux-gnu/lib
    ```

# Usage

If you open a `.rs` file you should see the code rainbow-highlighted, there should be content in *Outline* toolview, you can use code browser etc. Errors and warnings should be visible in *Problems* toolview. If there is some problem running the `rustc2duchain` tool, it will appear in the *Problems* toolview as well.
