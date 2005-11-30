typedef char *pointer;
void
qsort (register pointer *a, register int n, register int size, register int (*order) ())
{
register pointer *i, *j, *m, pi, pj, pm;
pointer v;
if (size != sizeof(pointer)) abort();
while (n > 4) {
i = a;
j = a + n - 1;
m = a + (n >> 1);
pi = *i;
pj = *j;
pm = *m;
if ((*order)(i, j) <= 0) {
if ((*order)(m, i) <= 0) {
v = pi;
*i = pm;
}
else if ((*order)(j, m) < 0) {
v = pj;
*j = pm;
} else {
v = pm;
}
}
else {
if ((*order)(m, j) < 0) {
v = pj;
*j = pi;
*i = pm;
}
else if ((*order)(i, m) < 0) {
v = pi;
*i = pj;
*j = pm;
}
else {
v = pm;
*i = pj;
*j = pi;
}
}
i += 1;
*m = *i;
for (;;) {
do {
j -= 1;
if (j == i) goto exitloop;
} while ((*order)(&v, j) < 0);
*i = *j;
do {
i += 1;
if (i == j) goto exitloop;
} while ((*order)(i, &v) < 0);
*j = *i;
}
exitloop:
*i = v;
if (i < m) {
if (i - 1 != a) {
qsort (a, i - a, sizeof(pointer), order);
}
n = (a + n) - (i + 1);
a = i + 1;
}
else {
if (i + 1 != a + n) {
qsort (i + 1, (a + n) - (i + 1), sizeof(pointer), order);
}
n = i - a;
}
}
if (n <= 1) return;
i = a + 1;
j = a + n;
do {
v = i[0];
if ((*order)(i - 1, &v) > 0) {
do {
i[0] = i[-1];
i -= 1;
} while (i != a && (*order)(i - 1, &v) > 0);
i[0] = v;
}
i += 1;
} while (i != j);
}
