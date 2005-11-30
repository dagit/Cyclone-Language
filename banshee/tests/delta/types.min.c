int *malloc_wrapper ()
{
  return (int *) malloc (sizeof (int));
}
void main ()
{
  Substitute (malloc_wrapper ());
}
void Substitute (int *new)
{
}
