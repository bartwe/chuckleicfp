
all:
	mkdir -p out/production/visualizer/
	javac -g -sourcepath src -d out/production/visualizer/ `find src -iname *.java`

run:
	cat  ../../maps/contest10.map | java -Xmx1G -cp out/production/visualizer DasaConsole
