/***************************************************************************
 *   Copyright (C) 2006 by Michael Ambrus                                  *
 *   michael.ambrus@maquet.com                                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

/*- include files **/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#undef NDEBUG
#include <assert.h>
#include <string.h>
#include <errno.h>

#define MAX_WORDS 15000
#define MAX_SORT_ELEMENT_SIZE 15000 //!< Limitation of each sorting elements size

#define TRUE 1
#define FALSE 0

typedef int my_comparison_fn_t(const void *L,   //!< <em>"Leftmost"</em> element to compare with
                               const void *R    //!< <em>"Rightmost"</em> element to compare with
    );

typedef my_comparison_fn_t *cmpf_t;

typedef struct myarg_t {
    void *a;                    //!< The array to be sorted
    int l;                      //!< left index
    int r;                      //!< right index
    int sz;                     //!< Size of each element
    cmpf_t cmpf;                //!< Comparison function
} myarg_t;

void my_swap(void *a,           //!< The array to be sorted
             int l,             //!< left index
             int r,             //!< right index
             int sz             //!< Size of each element
    )
{
    char t[MAX_SORT_ELEMENT_SIZE];
    memcpy(t, &((char *)a)[l * sz], sz);
    memcpy(&((char *)a)[l * sz], &((char *)a)[r * sz], sz);
    memcpy(&((char *)a)[r * sz], t, sz);
}

unsigned int my_qsort_depth = 0;    //!< @brief Recursion debug variable @internal
unsigned int my_curr_depth = 0; //!< @brief Recursion debug variable @internal

void *conc_quicksort(void *inarg_vp)
{
    int i, j;
    void *v;
    struct myarg_t *inarg_p = (myarg_t *) inarg_vp;
    struct myarg_t leftArg;
    struct myarg_t rightArg;
    pthread_t leftID;
    pthread_t rightID;

    my_curr_depth++;
    if (my_curr_depth > my_qsort_depth)
        my_qsort_depth = my_curr_depth;

    if (inarg_p->r > inarg_p->l) {

        v = (void **)(((char *)(inarg_p->a)) + (inarg_p->r * inarg_p->sz));
        i = inarg_p->l - 1;
        j = inarg_p->r;
        for (;;) {
            while (inarg_p->
                   cmpf((void **)(((char *)(inarg_p->a)) + (++i * inarg_p->sz)),
                        v) < 0) ;
            while (inarg_p->
                   cmpf((void **)(((char *)(inarg_p->a)) + (--j * inarg_p->sz)),
                        v) > 0) ;
            if (i >= j)
                break;
            my_swap(inarg_p->a, i, j, inarg_p->sz);
        }
        my_swap(inarg_p->a, i, inarg_p->r, inarg_p->sz);

        leftArg.a = inarg_p->a;
        leftArg.l = inarg_p->l;
        leftArg.r = i - 1;
        leftArg.cmpf = inarg_p->cmpf;
        leftArg.sz = inarg_p->sz;

        rightArg.a = inarg_p->a;
        rightArg.l = i + 1;
        rightArg.r = inarg_p->r;
        rightArg.cmpf = inarg_p->cmpf;
        rightArg.sz = inarg_p->sz;

        assert(pthread_create(&leftID, NULL, conc_quicksort, &leftArg) == 0);
        assert(pthread_create(&rightID, NULL, conc_quicksort, &rightArg) == 0);
        assert(pthread_join(leftID, NULL) == 0);
        assert(pthread_join(rightID, NULL) == 0);
    }
    my_curr_depth--;
    return (void *)0;
}

#define ISALFANUM( expr ) (\
   (expr >= 'A') && (expr <= 127) ? 1 : 0  \
)

//any nonzero expression is considered true in C.
int my_strvcmp(const void *L, const void *R)
{
    return strncmp(*(char **)L, *(char **)R, 80);
}

char *text_p[MAX_WORDS];
//------1---------2---------3---------4---------5---------6---------7---------8
//Text is embedded here for "sizeof" further below to work

#ifndef READ_TEXT
#define READ_TEXT "around_the_world_s.txt"
#endif

static const char theText[] =
    "                                                   \
AROUND THE WORLD IN EIGHTY DAYS                                                   \
                                                                                  \
                                                                                  \
                                                                                  \
Chapter I                                                                         \
                                                                                  \
IN WHICH PHILEAS FOGG AND PASSEPARTOUT ACCEPT EACH OTHER,                         \
THE ONE AS MASTER, THE OTHER AS MAN                                               \
                                                                                  \
                                                                                  \
Mr. Phileas Fogg lived, in 1872, at No. 7, Saville Row, Burlington                \
Gardens, the house in which Sheridan died in 1814.  He was one of                 \
the most noticeable members of the Reform Club, though he seemed                  \
always to avoid attracting attention; an enigmatical personage,                   \
about whom little was known, except that he was a polished man                    \
of the world.  People said that he resembled Byron--at least                      \
that his head was Byronic; but he was a bearded, tranquil Byron,                  \
who might live on a thousand years without growing old.                           \
                                                                                  \
Certainly an Englishman, it was more doubtful whether Phileas Fogg                \
was a Londoner.  He was never seen on 'Change, nor at the Bank,                   \
nor in the counting-rooms of the \"City\"; no ships ever came into                \
                                                                                  \
London docks of which he was the owner; he had no public employment;              \
he had never been entered at any of the Inns of Court, either at the Temple,      \
or Lincoln's Inn, or Gray's Inn; nor had his voice ever resounded                 \
in the Court of Chancery, or in the Exchequer, or the Queen's Bench,              \
or the Ecclesiastical Courts.  He certainly was not a manufacturer;               \
nor was he a merchant or a gentleman farmer.  His name was strange                \
to the scientific and learned societies, and he never was known                   \
to take part in the sage deliberations of the Royal Institution                   \
or the London Institution, the Artisan's Association, or the                      \
Institution of Arts and Sciences.  He belonged, in fact,                          \
to none of the numerous societies which swarm in the English capital,             \
from the Harmonic to that of the Entomologists, founded mainly                    \
for the purpose of abolishing pernicious insects.                                 \
                                                                                  \
Phileas Fogg was a member of the Reform, and that was all.                        \
                                                                                  \
The way in which he got admission to this exclusive club                          \
was simple enough.                                                                \
                                                                                  \
He was recommended by the Barings, with whom he had an open credit.               \
His cheques were regularly paid at sight from his account current,                \
which was always flush.                                                           \
                                                                                  \
Was Phileas Fogg rich?  Undoubtedly.  But those who knew him                      \
best could not imagine how he had made his fortune, and Mr. Fogg                  \
was the last person to whom to apply for the information.  He was                 \
not lavish, nor, on the contrary, avaricious; for, whenever he knew               \
that money was needed for a noble, useful, or benevolent purpose,                 \
he supplied it quietly and sometimes anonymously.  He was, in short,              \
the least communicative of men.  He talked very little, and seemed                \
all the more mysterious for his taciturn manner.  His daily habits                \
were quite open to observation; but whatever he did was so exactly                \
the same thing that he had always done before, that the wits                      \
of the curious were fairly puzzled.                                               \
                                                                                  \
Had he travelled?  It was likely, for no one seemed to know                       \
the world more familiarly; there was no spot so secluded                          \
that he did not appear to have an intimate acquaintance with it.                  \
He often corrected, with a few clear words, the thousand conjectures              \
advanced by members of the club as to lost and unheard-of travellers,             \
pointing out the true probabilities, and seeming as if gifted with                \
a sort of second sight, so often did events justify his predictions.              \
He must have travelled everywhere, at least in the spirit.                        \
";/*
                                                                                  \
It was at least certain that Phileas Fogg had not absented himself                \
from London for many years.  Those who were honoured by a better                  \
acquaintance with him than the rest, declared that nobody could                   \
pretend to have ever seen him anywhere else.  His sole pastimes                   \
were reading the papers and playing whist.  He often won at this game,            \
which, as a silent one, harmonised with his nature; but his winnings              \
never went into his purse, being reserved as a fund for his charities.            \
Mr. Fogg played, not to win, but for the sake of playing.                         \
The game was in his eyes a contest, a struggle with a difficulty,                 \
yet a motionless, unwearying struggle, congenial to his tastes.                   \
                                                                                  \
Phileas Fogg was not known to have either wife or children,                       \
which may happen to the most honest people; either relatives                      \
or near friends, which is certainly more unusual.  He lived alone                 \
in his house in Saville Row, whither none penetrated.  A single                   \
at hours mathematically fixed, in the same room, at the same table,               \
never taking his meals with other members, much less bringing                     \
a guest with him; and went home at exactly midnight, only to retire               \
at once to bed.  He never used the cosy chambers which the Reform                 \
provides for its favoured members.  He passed ten hours out of the                \
twenty-four in Saville Row, either in sleeping or making his toilet.              \
When he chose to take a walk it was with a regular step in the                    \
entrance hall with its mosaic flooring, or in the circular gallery                \
with its dome supported by twenty red porphyry Ionic columns,                     \
and illumined by blue painted windows.  When he breakfasted or dined              \
all the resources of the club--its kitchens and pantries,                         \
its buttery and dairy--aided to crowd his table with their most                   \
succulent stores; he was served by the gravest waiters,                           \
in dress coats, and shoes with swan-skin soles, who proffered                     \
the viands in special porcelain, and on the finest linen;                         \
club decanters, of a lost mould, contained his sherry,                            \
his port, and his cinnamon-spiced claret; while his beverages                     \
were refreshingly cooled with ice, brought at great cost                          \
from the American lakes.                                                          \
                                                                                  \
If to live in this style is to be eccentric, it must be                           \
confessed that there is something good in eccentricity.                           \
                                                                                  \
The mansion in Saville Row, though not sumptuous, was exceedingly comfortable.    \
The habits of its occupant were such as to demand but little from the             \
sole domestic, but Phileas Fogg required him to be almost superhumanly            \
prompt and regular.  On this very 2nd of October he had dismissed                 \
James Forster, because that luckless youth had brought him shaving-water          \
at eighty-four degrees Fahrenheit instead of eighty-six;                          \
and he was awaiting his successor, who was due at the house                       \
between eleven and half-past.                                                     \
                                                                                  \
Phileas Fogg was seated squarely in his armchair, his feet close together         \
like those of a grenadier on parade, his hands resting on his knees,              \
his body straight, his head erect; he was steadily watching a complicated         \
clock which indicated the hours, the minutes, the seconds, the days,              \
the months, and the years.  At exactly half-past eleven Mr. Fogg would,           \
according to his daily habit, quit Saville Row, and repair to the Reform.         \
                                                                                  \
A rap at this moment sounded on the door of the cosy apartment where              \
Phileas Fogg was seated, and James Forster, the dismissed servant, appeared.      \
                                                                                  \
\"The new servant,\" said he.                                                     \
                                                                                  \
A young man of thirty advanced and bowed.                                         \
                                                                                  \
\"You are a Frenchman, I believe,\" asked Phileas Fogg, \"and your name is John?\"\
                                                                                  \
\"Jean, if monsieur pleases,\" replied the newcomer, \"Jean Passepartout,         \
a surname which has clung to me because I have a natural aptness                  \
for going out of one business into another.  I believe I'm honest,                \
monsieur, but, to be outspoken, I've had several trades.  I've been               \
an itinerant singer, a circus-rider, when I used to vault like Leotard,           \
and dance on a rope like Blondin.  Then I got to be a professor of gymnastics,    \
so as to make better use of my talents; and then I was a sergeant fireman         \
at Paris, and assisted at many a big fire.  But I quitted France                  \
five years ago, and, wishing to taste the sweets of domestic life,                \
took service as a valet here in England.  Finding myself out of place,            \
and hearing that Monsieur Phileas Fogg was the most exact and settled             \
gentleman in the United Kingdom, I have come to monsieur in the hope              \
of living with him a tranquil life, and forgetting even the name                  \
of Passepartout.\"                                                                \
                                                                                  \
                                                                                  \
";
*/
//------1---------2---------3---------4---------5---------6---------7---------8
/* Make test shorter for weaker targets */
#ifdef __ANDROID__
#define NLOOPS 1000
#endif
/* Default to something...*/
#ifndef NLOOPS
#define NLOOPS 50000
#endif
int run_test(int j)
{
    pthread_t sortID;
    int i, k;
    int rc;
    struct timespec tp;
    struct myarg_t sortArg;

    //for (l=0; l<n; l++){
    //Use normal qsort as a reference (enable the following row and disable the thread below)
    //qsort(text_p,j-1,sizeof(char*),my_strvcmp);
    rc = clock_gettime(CLOCK_MONOTONIC_RAW, &tp);
    assert(rc == 0);
    fprintf(stderr, "%d.%09d ", (int)tp.tv_sec, (int)tp.tv_nsec);

    sortArg.a = text_p;
    sortArg.l = 0;
    sortArg.r = j - 2;
    sortArg.sz = sizeof(char *);
    sortArg.cmpf = my_strvcmp;

    assert(pthread_create(&sortID, NULL, conc_quicksort, &sortArg) == 0);
    assert(pthread_join(sortID, NULL) == 0);

    for (i = 0, k = 0; i < j; i++) {
        assert(j < NLOOPS);
        for (k = 0; ISALFANUM(text_p[i][k]); k++)
            putchar(text_p[i][k]);
        printf("\n");
    }
#if defined(__TINKER)
    printf("\n\n========\nJob done. Total number of threads: %d\n",
           some_internal_cntr);
#endif
    rc = clock_gettime(CLOCK_MONOTONIC_RAW, &tp);
    assert(rc == 0);
    fprintf(stderr, "%d.%09d\n", (int)tp.tv_sec, (int)tp.tv_nsec);
    //}
    return 0;
}

int main(int argc, char **argv)
{
    unsigned int i, j, l;
    int inText = 0;
    char *backup;

#if defined(TINKER)
    printf("Tinker concurrent sort\n");
#else
    printf("Linux concurrent sort\n");
#endif
    printf("Root started\n");

    //Build up the pointer table (primitive, yes I know ;) )
    for (i = 0, j = 0; i < sizeof(theText); i++) {
        assert(j < MAX_WORDS);

        if ((inText == FALSE) && ISALFANUM(theText[i])) {
            inText = 1;
            text_p[j] = (char *)&theText[i];
            j++;
        }

        if ((inText == TRUE) && !ISALFANUM(theText[i])) {
            inText = 0;
        }
    }
    backup = malloc(sizeof text_p);
    memcpy(backup, text_p, sizeof text_p);

    for (l = 0; l < NLOOPS; l++) {
        memcpy(text_p, backup, sizeof text_p);
        run_test(j);
    }
    printf("Finished successfully\n");

    return 0;
}
