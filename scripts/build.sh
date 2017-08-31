

if [ ! -f genie ]; then
    if [ "$(uname)" == "Darwin" ]; then
        wget https://github.com/bkaradzic/bx/raw/master/tools/bin/darwin/genie
    elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
        wget https://github.com/bkaradzic/bx/raw/master/tools/bin/linux/genie
    fi
    chmod +x genie
fi

if [ "$#" -eq 0 ]; then
    echo No argument set
	echo Exemple : build.bat --with-opengl gmake
    exit 1
fi

./genie $@