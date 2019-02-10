## Understandable RayTracing

Based on https://github.com/ssloy/tinyraytracer. Read the [wiki]
for more details.

### Setup

- The default branch is *devel*.
- Releases are made from *master*, which has
a GitHub branch protection rule that requires a pull request review
from [Code Owners] before merging.
- The [Pitchfork project layout] is used.
- The build directory is *build/*.
- [ClangFormat] is configured with .clang-format
- Build script in *tools/*.
- Use `cmake-format -i CMakeLists.txt` to [format CMakeLists.txt].
- `clang-tidy.sh` in *tools/* to run [ClangTidy].

[Code Owners]: https://help.github.com/articles/about-code-owners/
[Pitchfork project layout]: https://api.csswg.org/bikeshed/?force=1&url=https://raw.githubusercontent.com/vector-of-bool/pitchfork/develop/data/spec.bs#tld.docs
[wiki]: https://github.com/ssloy/tinyraytracer/wiki
[ClangFormat]: https://clang.llvm.org/docs/ClangFormat.html
[format CMakeLists.txt]: https://pypi.org/project/cmake_format/
[ClangTidy]: https://clang.llvm.org/extra/clang-tidy/