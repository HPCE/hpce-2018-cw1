#!/bin/bash

if [ "$#" -ne 2 ]
then
  echo "Usage: ./submission.sh IMPERIAL_LOGIN PATH_TO_ZIP"
  exit 1
fi

STUDENT_LOGIN=$1;
ZIP_FILE=$2;


if [ ! -f ${ZIP_FILE} ]; then
    echo "Error (Fatal) : Zip file ${ZIP_FILE} doesn't exist.";
    exit 1;
fi

# Create a temporary working directory
mkdir -p ./working
WORKING_DIR=$(mktemp -d ./working/XXXXXX)
echo "Working directory is ${WORKING_DIR}"


function check_for_tool {
    which $1 > /dev/null;
    if [[ $? -ne 0 ]]; then
        echo "Error (Fatal) : it looks like the tool $1 is not installed."
        exit 1
    fi
}

# Look for a few tools that we will need in this script
check_for_tool unzip;
check_for_tool awk;
check_for_tool grep;
check_for_tool diff;
check_for_tool g++;
check_for_tool make;

# Progress will be written to this file
LOG_FILE=${WORKING_DIR}/${STUDENT_LOGIN}.log

# Extract the zip file there
# If this fails, then you might need to install `zip`
echo "Unzipping into ${WORKING_DIR}"
unzip -q ${ZIP_FILE} -d ${WORKING_DIR}
if [ $? -ne 0 ]; then
    echo "Error (Fatal) : Couldn't unzip ${ZIP_FILE}."
    exit 1;
fi

# This is needed because people put content in zip
# files at different levels, so we need to find the
# anchor point. In this case I look for the original
# readme.md, and consider all sub-directories of the
# original.
echo "Looking for the readme.md file."
ANCHOR_DIR=""
if [ -f ${WORKING_DIR}/readme.md ]; then
    ANCHOR_DIR=${WORKING_DIR};
else
    SUBDIRS=$(ls ${WORKING_DIR});
	for i in $SUBDIRS; do
		echo "  Looking in $i";
		if [[ -f ${WORKING_DIR}/$i/makefile ]]; then
			echo "   Got it.";
			ANCHOR_DIR="${WORKING_DIR}/${i}";
			break;
		fi;
	done;
	if [[ "" = "$ANCHOR_DIR" ]]; then
		echo "Error (Fatal) : Couldn't find the readme.md in the zip."
		exit 1;
	fi;
fi;

echo "Cleaning the bin directory (just in case)."
rm -f ${ANCHOR_DIR}/bin/julia

echo "Force building bin/julia."
(cd ${ANCHOR_DIR} && make -B bin/julia)
if [[ $? -ne 0 ]]; then
    echo "Warning : `make bin/julia` seems to have failed.";
fi
if [[ ! -f ${ANCHOR_DIR}/bin/julia ]]; then
    echo "Error (Fatal) : `make bin/julia` did not result in a file called ${ANCHOR_DIR}/bin/julia";
    exit 1;
fi

##################################################
## We should now definitely have a binary, and can
## look at the other stuff.

test_index=0;
test_passed=0;

function log_base {
	echo $1 >> $LOG_FILE;
	echo $1
}

function test_header {
    test_index=$(($test_index+1));
    echo "";
	echo "##############################################################";
	echo "## Begin test $test_index";
	echo "";
}

function test_footer {
    echo "";
	echo "## End test $test_index";
	echo "##############################################################";
	echo "";
}

# Report whether a test passed or failed
# $1 - Status code (0 = passed, 1 = failed)
# $2 - Test name
function test_report {
    if [[ "$1" -ne 0 ]]; then
        log_base "FAIL, $2"
    else
        log_base "Pass, $2"
        test_passed=$(($test_passed+1));
    fi
}

# Run a command, check that it succeeds
# $1 - Name of test
# $2 - Command to run
function test_command_and_exit_code {
    local R
	test_header;
	eval "$2";
    R="$?"
    echo ""
    test_report $R "$1";
	test_footer;
}

# Run a condition, check that it is true
# $1 - Name of test
# $2 - Condition to check
function test_condition {
    local failed
	test_header;
    if [[ "$2" -ne 0 ]]; then
        failed=0
    else
        failed=1
    fi
    test_report $failed "$1 ($2)";
	test_footer;

}

function test_pdf_exists {
    test_header;
    local failed
    failed=1
    if [[ -f "${ANCHOR_DIR}/$1" ]]; then
        failed=0;
    fi
    test_report $failed "Check for existence of file $1.";
    test_footer;
}


cat << ReferenceFile16x16 > ${ANCHOR_DIR}/julia_16x16.ref
.:::::::::::::::
::::::::::::::::
:::::::-------::
:::::--=++==----
:::---=+  **++=-
::---=+*#    %#=
:---=+*@
:--+* =        +
-==*         *==
-+        = *+--
-        @*+=---
-=#%    #*+=---:
:-=++**  +=---::
:----==++=--::::
:::-------::::::
::::::::::::::::
ReferenceFile16x16


ENGINES="parallel_inner parallel_outer parallel_both";

for e in ${ENGINES}; do
    test_command_and_exit_code \
        "${e} calls tbb::parallel_for" \
        "grep -C 2 tbb::parallel_for ${ANCHOR_DIR}/src/julia_frame_${e}.cpp "

    test_command_and_exit_code \
        "${e} executes correctly" \
        "${ANCHOR_DIR}/bin/julia -engine ${e} -width 16 -height 16 > ${ANCHOR_DIR}/${e}_16x16.got  "

    test_command_and_exit_code \
        "${e} output is correct" \
        "diff --ignore-space-change ${ANCHOR_DIR}/julia_16x16.ref ${ANCHOR_DIR}/${e}_16x16.got"
done

# This is to work around the vagaries of `time` on different shells
function time_command_ms {
    BEGIN=$(date "+%s.%N");
    eval $1;
    END=$(date "+%s.%N");
    awk "BEGIN {printf \"%d\", ( ${END} - ${BEGIN} ) * 1000 }"
}

WIDE_REF=$(time_command_ms "${ANCHOR_DIR}/bin/julia -width 1000 -height 2 -max-iter 100000 > /dev/null")
WIDE_INNER=$(time_command_ms "${ANCHOR_DIR}/bin/julia -engine parallel_inner -width 1000 -height 2 -max-iter 100000 > /dev/null")
WIDE_OUTER=$(time_command_ms "${ANCHOR_DIR}/bin/julia -engine parallel_outer -width 1000 -height 2 -max-iter 100000 > /dev/null")
WIDE_BOTH=$(time_command_ms "${ANCHOR_DIR}/bin/julia -engine parallel_both -width 1000 -height 2 -max-iter 100000 > /dev/null")

test_condition "For wide output reference time ${WIDE_REF} similar to parallel_outer ${WIDE_OUTER}" \
    "$(( ( ${WIDE_REF} * 3 < ${WIDE_OUTER} * 4 ) && ( ${WIDE_REF} * 4 > ${WIDE_OUTER} * 3 ) ))"

test_condition "For wide output reference time ${WIDE_REF} slower than parallel_inner ${WIDE_INNER}" \
    "$(( ${WIDE_REF} * 3 > ${WIDE_INNER} * 4 ))"

test_condition "For wide output parallel_both time ${WIDE_BOTH} similar to parallel_inner ${WIDE_INNER}" \
    "$(( ( ${WIDE_BOTH} * 3 < ${WIDE_INNER} * 4 ) && ( ${WIDE_BOTH} * 4 > ${WIDE_INNER} * 3 ) ))"

TALL_REF=$(time_command_ms "${ANCHOR_DIR}/bin/julia -width 2 -height 1000 -max-iter 100000 > /dev/null")
TALL_INNER=$(time_command_ms "${ANCHOR_DIR}/bin/julia -engine parallel_inner -width 2 -height 1000 -max-iter 100000 > /dev/null")
TALL_OUTER=$(time_command_ms "${ANCHOR_DIR}/bin/julia -engine parallel_outer -width 2 -height 1000 -max-iter 100000 > /dev/null")
TALL_BOTH=$(time_command_ms "${ANCHOR_DIR}/bin/julia -engine parallel_both -width 2 -height 1000 -max-iter 100000 > /dev/null")

test_condition "For tall output reference time ${TALL_REF} similar to parallel_inner ${TALL_INNER}" \
    "$(( ( ${TALL_REF} * 3 < ${TALL_INNER} * 4 ) && ( ${TALL_REF} * 4 > ${TALL_INNER} * 3 ) ))"

test_condition "For tall output reference time ${TALL_REF} slower than parallel_outer ${TALL_OUTER}" \
    "$(( ${TALL_REF} * 3 > ${TALL_OUTER} * 4 ))"

test_condition "For tall output parallel_both time ${TALL_BOTH} similar to parallel_outer ${TALL_OUTER}" \
    "$(( ( ${TALL_BOTH} * 3 < ${TALL_OUTER} * 4 ) && ( ${TALL_BOTH} * 4 > ${TALL_OUTER} * 3 ) ))"



test_pdf_exists "results/max_iter_versus_time.pdf"

test_pdf_exists "results/dimension_versus_time.pdf"

test_pdf_exists "results/release_max_iter_versus_time.pdf"

test_pdf_exists "results/release_dimension_versus_speedup.pdf"

test_pdf_exists "results/scaling_max_iter.pdf"

test_pdf_exists "results/scaling_dimension.pdf"

test_pdf_exists "results/scaling_width.pdf"

test_pdf_exists "results/scaling_height.pdf"

test_pdf_exists "results/scaling_max_frames.pdf"


log_base "Passed ${test_passed} out of ${test_index} tests.";
echo "Output log is in ${LOG_FILE}";


