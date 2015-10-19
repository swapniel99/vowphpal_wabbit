# vowphpal_wabbit
A PHP wrapper for Vowpal Wabbit

Prerequisites:
Vowpal Wabbit : https://github.com/JohnLangford/vowpal_wabbit/wiki
PHP-CPP : http://www.php-cpp.com/

Only a variable can be passed into a function and NOT a literal.

For eg. 
```
f("hello");  <--  is not correct.
```
instead do: 
```
$h = "hello";
f($h);
```

Example usage in PHP:
```
php> echo VowPHPal_Wabbit::isModelPresent();
0

php> $init = "--quiet -t -i /path/to/modelfile";
php> VowPHPal_Wabbit::initializeStaticModel($init);
php> echo VowPHPal_Wabbit::isModelPresent();
1

php> $test = "-1 |n1 a b c |n2 b:4";
php> echo VowPHPal_Wabbit::getPrediction($test);
0.3

php> $test2 = array("-1 |n1 a b c |n2 b:4","-1 |n1 a b c |n2 b:4");
php> $res = VowPHPal_Wabbit::getnPredictions($test2);
php> echo $res[0];
0.3

php> echo VowPHPal_Wabbit::getCounter();
3

php> VowPHPal_Wabbit::finishStaticModel();
```
Once model is finished scores cannot be predicted. It always returns 0.
If a new model is initialised before finishing old model, old model is automatically finished.

In case of apache prefork, once model is initialsed it will be used again and again without reading model file from disk each time.
The total number of model file reads with one initialise per thread = number of apache threads
Also the counter will be maintained separately for each apache thread.

So in PHP simply check if a model exists and use it if it is present. Load a new model if necessary. 

NOTES:
I have only used this setup with -t option until now. But I am not sure at this point whether it can be used for training.
-r option doesn't work here.
Since this loads model file directly into memory, please verify the integrity of model file. Theoretically it is vulnerable to injection.

