cmake . -DCMAKE_INSTALL_PREFIX=$PREFIX -DCMAKE_INSTALL_INCLUDEDIR=include \
    -DCMAKE_INSTALL_LIBDIR=lib -DBUILD_TESTS=OFF -DBUILD_EXAMPLES=OFF -Bbuild

cmake --build build/ --target install


