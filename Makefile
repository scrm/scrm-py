demo.cpython-36m-x86_64-linux-gnu.so: scrm-py.cpp
	python setup.py build_ext -i

dist: scrm-py.cpp MANIFEST.in setup.py
	python setup.py build sdist

clean:
	rm -fR build *.so

