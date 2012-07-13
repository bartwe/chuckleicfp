to compile

mkdir -p out/production/visualizer/
javac -g -sourcepath src -d out/production/visualizer/ `find src -iname
*.java`



to run:

java -ea -cp out/production/visualizer DisplayWorld ../../omni/contest10.map
