project(
	base64
	HOMEPAGE_URL https://github.com/zhicheng/base64/
	LANGUAGES C
)

add_library(
	${PROJECT_NAME} STATIC
	./base64.h
	./base64.c
	./main.c
)