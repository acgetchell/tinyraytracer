## Understandable RayTracing

[Dmitry Sokolov]'s [tinyraytracer], with some changes.
Read the [wiki] for more details, and especially [Ray-Sphere Intersection].

### Project Setup

- The default branch is *devel*.
- Releases are made from *master*, which has
a GitHub branch protection rule that requires a pull request review
from [Code Owners] before merging.
- The [Pitchfork project layout] is used.
- The build directory is *build/*.
- [ClangFormat] is configured with .clang-format
- Build script [build.sh] in *tools/*.
- Use `cmake-format -i CMakeLists.txt` to [format CMakeLists.txt].
- `clang-tidy.sh` in *tools/* to run [ClangTidy].
- Turn on many [helpful compiler warnings].

### Usage

```bash
git clone git@github.com:acgetchell/tinyraytracer.git
cd tinyraytracer/tools
./build.sh
```
This generates an executable `tinyraytracer` with an output of `out.ppm`
in *build/*. It also runs `ctest`, which checks that the program compiles
correctly, and that the output file exists.


[Dmitry Sokolov]: https://github.com/ssloy
[tinyraytracer]: https://github.com/ssloy/tinyraytracer
[Code Owners]: https://help.github.com/articles/about-code-owners/
[Pitchfork project layout]: https://api.csswg.org/bikeshed/?force=1&url=https://raw.githubusercontent.com/vector-of-bool/pitchfork/develop/data/spec.bs#tld.docs
[wiki]: https://github.com/ssloy/tinyraytracer/wiki
[ClangFormat]: https://clang.llvm.org/docs/ClangFormat.html
[format CMakeLists.txt]: https://pypi.org/project/cmake_format/
[ClangTidy]: https://clang.llvm.org/extra/clang-tidy/
[Ray-Sphere Intersection]: http://www.lighthouse3d.com/tutorials/maths/ray-sphere-intersection/
[helpful compiler warnings]: https://github.com/acgetchell/tinyraytracer/blob/devel/CMakeLists.txt#L33
[build.sh]: https://github.com/acgetchell/tinyraytracer/blob/devel/tools/build.sh