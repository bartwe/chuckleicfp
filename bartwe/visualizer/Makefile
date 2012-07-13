
all:
	mkdir -p out/production/visualizer/
	javac -g -sourcepath src -d out/production/visualizer/ `find src -iname *.java`

run:
	java -ea -cp out/production/visualizer Manual ../../maps/contest10.map
