build: sdist mac_wheel linux_wheel

sdist:
	python3 setup.py sdist

mac_wheel:
	python3.7 setup.py bdist_wheel
	python3.8 setup.py bdist_wheel
	python3.9 setup.py bdist_wheel
	python3.10 setup.py bdist_wheel
	python3.11 setup.py bdist_wheel

linux_wheel:
	docker run -it --rm \
		-v `pwd`:/app \
		--entrypoint /bin/bash \
		quay.io/pypa/manylinux2014_aarch64 \
		/app/scripts/make-wheels.sh

	docker run -it --rm \
		-v `pwd`:/app \
		--entrypoint /bin/bash \
		quay.io/pypa/manylinux2014_x86_64 \
		/app/scripts/make-wheels.sh
