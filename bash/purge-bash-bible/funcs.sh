set -e 

trim_string() {
    : "${1#"${1%%[![:space:]]*}"}"
    : "${_%"${_##*[![:space:]]}"}"
    printf '%s\n' "$_"
}

trim_all() {
    set -f
    set -- $*
    printf '%s\n' "$*"
    set +f
}

regex() {
    [[ $1 =~ $2 ]] && printf '%s\n' "${BASH_REMATCH[1]}"
}

split() {
   IFS=$'\n' read -d "" -ra arr <<< "${1//$2/$'\n'}"
   printf '%s\n' "${arr[@]}"
}

lower() {
    printf '%s\n' "${1,,}"
}

upper() {
    printf '%s\n' "${1^^}"
}

reverse_case() {
    printf '%s\n' "${1~~}"
}

trim_quotes() {
    : "${1//\'}"
    printf '%s\n' "${_//\"}"
}

strip_all() {
    printf '%s\n' "${1//$2}"
}

strip() {
    printf '%s\n' "${1/$2}"
}

lstrip() {
    printf '%s\n' "${1##$2}"
}

rstrip() {
    printf '%s\n' "${1%%$2}"
}

urlencode() {
    local LC_ALL=C
    for (( i = 0; i < ${#1}; i++ )); do
        : "${1:i:1}"
        case "$_" in
            [a-zA-Z0-9.~_-])
                printf '%s' "$_"
            ;;

            *)
                printf '%%%02X' "'$_"
            ;;
        esac
    done
    printf '\n'
}

urldecode() {
    : "${1//+/ }"
    printf '%b\n' "${_//%/\\x}"
}

reverse_array() {
    shopt -s extdebug
    f()(printf '%s\n' "${BASH_ARGV[@]}"); f "$@"
    shopt -u extdebug
}

remove_array_dups() {
    declare -A tmp_array

    for i in "$@"; do
        [[ $i ]] && IFS=" " tmp_array["${i:- }"]=1
    done

    printf '%s\n' "${!tmp_array[@]}"
}

random_array_element() {
    local arr=("$@")
    printf '%s\n' "${arr[RANDOM % $#]}"
}

arr=(a b c d)

cycle() {
    printf '%s ' "${arr[${i:=0}]}"
    ((i=i>=${#arr[@]}-1?0:++i))
}

arr=(true false)

cycle() {
    printf '%s ' "${arr[${i:=0}]}"
    ((i=i>=${#arr[@]}-1?0:++i))
}

head() {
    mapfile -tn "$1" line < "$2"
    printf '%s\n' "${line[@]}"
}

tail() {
    mapfile -tn 0 line < "$2"
    printf '%s\n' "${line[@]: -$1}"
}

lines() {
    mapfile -tn 0 lines < "$1"
    printf '%s\n' "${#lines[@]}"
}

lines_loop() {
    count=0
    while IFS= read -r _; do
        ((count++))
    done < "$1"
    printf '%s\n' "$count"
}

count() {
    printf '%s\n' "$#"
}

extract() {
    while IFS=$'\n' read -r line; do
        [[ $extract && $line != "$3" ]] &&
            printf '%s\n' "$line"

        [[ $line == "$2" ]] && extract=1
        [[ $line == "$3" ]] && extract=
    done < "$1"
}

dirname() {
    local tmp=${1:-.}

    [[ $tmp != *[!/]* ]] && {
        printf '/\n'
        return
    }

    tmp=${tmp%%"${tmp##*[!/]}"}

    [[ $tmp != */* ]] && {
        printf '.\n'
        return
    }

    tmp=${tmp%/*}
    tmp=${tmp%%"${tmp##*[!/]}"}

    printf '%s\n' "${tmp:-/}"
}

basename() {
    local tmp

    tmp=${1%"${1##*[!/]}"}
    tmp=${tmp##*/}
    tmp=${tmp%"${2/"$tmp"}"}

    printf '%s\n' "${tmp:-/}"
}

get_term_size() {

    shopt -s checkwinsize; (:;:)
    printf '%s\n' "$LINES $COLUMNS"
}

get_window_size() {
    printf '%b' "${TMUX:+\\ePtmux;\\e}\\e[14t${TMUX:+\\e\\\\}"
    IFS=';t' read -d t -t 0.05 -sra term_size
    printf '%s\n' "${term_size[1]}x${term_size[2]}"
}

get_cursor_pos() {
    IFS='[;' read -p $'\e[6n' -d R -rs _ y x _
    printf '%s\n' "$x $y"
}

hex_to_rgb() {
    : "${1/\#}"
    ((r=16#${_:0:2},g=16#${_:2:2},b=16#${_:4:2}))
    printf '%s\n' "$r $g $b"
}

rgb_to_hex() {
    printf '#%02x%02x%02x\n' "$1" "$2" "$3"
}

read_sleep() {
    read -rt "$1" <> <(:) || :
}

date() {
    printf "%($1)T\\n" "-1"
}

uuid() {
    C="89ab"

    for ((N=0;N<16;++N)); do
        B="$((RANDOM%256))"

        case "$N" in
            6)  printf '4%x' "$((B%16))" ;;
            8)  printf '%c%x' "${C:$RANDOM%${#C}:1}" "$((B%16))" ;;

            3|5|7|9)
                printf '%02x-' "$B"
            ;;

            *)
                printf '%02x' "$B"
            ;;
        esac
    done

    printf '\n'
}

bar() {
    ((elapsed=$1*$2/100))

    printf -v prog  "%${elapsed}s"
    printf -v total "%$(($2-elapsed))s"

    printf '%s\r' "[${prog// /-}${total}]"
}

get_functions() {
    IFS=$'\n' read -d "" -ra functions < <(declare -F)
    printf '%s\n' "${functions[@]//declare -f }"
}

bkr() {
    (nohup "$@" &>/dev/null &)
}

bkr ./some_script.sh # some_script.sh is now running in the background

to_upper() {
  local -n ptr=${1}

  ptr=${ptr^^}
}

foo="bar"
to_upper foo
printf "%s\n" "${foo}" # BAR

