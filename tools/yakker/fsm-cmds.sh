PATH=~/projects/fsm/bin:$PATH

cat yk_inputs.sym $1.sym >| yk.sym

echo Compiling input $1.fsm to $1_nfa.fsm
fsmcompile -s log -i yk.sym $1.fsm >| $1_nfa.fsm

echo Removing epsilons from $1_nfa.fsm "(output: $1_nfa_noeps.fsm)"
fsmrmepsilon $1_nfa.fsm >| $1_nfa_noeps.fsm

echo Determinizing $1_nfa_eps "(output: $1_dfa.fsm)"
fsmdeterminize $1_nfa_noeps.fsm >| $1_dfa.fsm

echo Printing $1_dfa.txt
fsmprint -i yk.sym $1_dfa.fsm >| $1_dfa.txt
