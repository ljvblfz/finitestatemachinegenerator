Like flex, yacc or bison, a new syntax for describe a finite state machine and generate the optimum code.

```
%%
 $START : '['
        {
          return($NEXT);
        }
        | /* Default */
        {
          return(-1);
        }
        ;
 $NEXT : ']'
        {
          return($START);
        }
        | /* Default */
        {
          return($state$);
        }
        ;
%%
int main()
{
  int state;
  for (state=0; state!=-1; state=fsm(state,getch()));
  return 0;
}
```