function cleanup() {
    tput cnorm;
    echo "";
}

tput civis;
trap cleanup EXIT;

gcc -o main -lm ee23b035_ode.c;

rm rk45_tau.dat -f;

max=100;
let offset=5;
let base=$max-$offset;
for ((i = 0 ; i <= max - offset; i++));
do
    alpha=`bc <<< "scale=5; 0.2 * ($i + $offset) / $max"`;
    let percent=($i*100)/$base;
    printf "\r|";
    for ((j = 0; j < $percent - 1; j++));
    do 
        printf "■";
    done;
    
    if [ $percent -eq 100 ]; then
        printf "■";
    else
        printf "▶";
    fi;

    for ((j = $percent ; j < 100; j++));
    do
        printf " ";
    done;

    printf "| Alpha = 0$alpha";

    tau=`./main 0.1745329 2.96706 $alpha 0.1`;
    echo 0$alpha $tau >> rk45_tau.dat;
done;
echo "";
