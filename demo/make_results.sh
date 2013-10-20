for f in *.jpg 
do
	echo "Processing $f..."
	./Sartroscope $f trees.jpg "result--$f"
done
