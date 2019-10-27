#!/bin/sh
#
# Author: Hans Liljestrand <hans.liljestrand@aalto.fi>
# Copyright: Secure Systems Group, Aalto University, https://ssg.aalto.fi
#
# This code is released under Apache 2.0 license
#

tests="arrays"
tests="$tests arrays-dummy"
tests="$tests stack_reordering"
fails="hack01"
xfails=""

FAILED=""
XFAILS=""
XFAILSOK=""
MISSING=""

run_test() {
    if [ -e "./$1" ]; then
        if [ -n "$2" ]; then
            if "./$1" "$2" > /dev/null 2>&1; then
                echo "$1 $2... OKAY!"
            else
                echo "$1 $2... FAILED!"
                FAILED="$FAILED\n\t$1 $2"
            fi
        else
            if "./$1" > /dev/null 2>&1; then
                echo "$1... OKAY!"
            else
                echo "$1... FAILED!"
                FAILED="$FAILED\n\t$1 "
            fi
        fi
    else
        echo "$1... MISSING!"
        MISSING="$MISSING\n\t$1* "
    fi
}

run_xfail() {
    if [ -e "./$1" ]; then
        if [ -n "$2" ]; then
            if "./$1" "$2" > /dev/null 2>&1; then
                echo "$1 $2... UNEXPECTED OKAY!"
                XFAILSOK="$XFAILSOK\n\t$1 $2"
            else
                echo "$1 $2... FAILED"
                XFAILS="$XFAILS\n\t$1 $2"
            fi
        else
            if "./$1" > /dev/null 2>&1; then
                echo "$1... UNEXPECTED OKAY!"
                XFAILSOK="$XFAILSOK\n\t$1 "
            else
                echo "$1... FAILED"
                XFAILS="$XFAILS\n\t$1 "
            fi
        fi
    else
        echo "$1... MISSING!"
        MISSING="$MISSING\n\t$1* "
    fi
}

run_fail_test() {
    if [ -e "./$1" ]; then
        if [ -n "$2" ]; then
            if "./$1" "$2" > /dev/null 2>&1; then
                echo "$1 $2... FAILED TO FAIL!"
                FAILED="$FAILED\n\t!$1 $2"
            else
                echo "$1 $2... OKAY!"
            fi
        else
            if "./$1" > /dev/null 2>&1; then
                echo "$1 $2... FAILED TO FAIL!"
                FAILED="$FAILED\n\t!$1 "
            else
                echo "$1... OKAY!"
            fi
        fi
    else
        echo "$1... MISSING!"
        MISSING="$MISSING\n\t$1* "
    fi
}

for test in $fails; do
	run_fail_test $test.out
done

for test in $tests; do
	run_test $test.out
done

for test in $xfails; do
	run_xfail $test.out
done

if [ ! -z "$XFAILS" ]; then
    echo ""
    echo "  expected failures"
    echo "  -----------------"
    echo -e "$XFAILS"
fi

if [ ! -z "$MISSING" ]; then
    echo ""
    echo "  #######################"
    echo "  # MISSING INPUT FILES #"
    echo "  #######################"
    echo ""
    echo -e "$MISSING"
fi

if [ ! -z "$XFAILSOK" ]; then
    echo ""
    echo "  ######################"
    echo "  # UNEXPECTED SUCCESS #"
    echo "  ######################"
    echo ""
    echo -e "$XFAILSOK"
fi

if [ ! -z "$FAILED" ]; then
    echo ""
    echo "  #####################"
    echo "  # FAILED SOME TESTS #"
    echo "  #####################"
    echo ""
    echo -e "$FAILED"
fi

