#include "header.h"
#include "agent_a_agent_header.h"

/** \fn agent_a_1()
 * \brief A description of the function
 */
int agent_a_1()
{
    int x, y, z;

    /* Add messages 0-9 for 3 axis' */
    for (x = 0; x <= 10; x++)
    {
        for (y = 0; y <= 10; y++)
        {
            for (z = 0; z <= 10; z++)
            {
                add_m_sub_1_message(x, y, z);
                add_m_sub_2_message((double)x, (double)y);
                add_m_sub_3_message((double)x, (double)y, (double)z);
            }
        }
    }

	return 0;
}

/** \fn agent_a_2()
 * \brief A description of the function
 */
int agent_a_2()
{
    int i;
    int mx, my, mz;
    int x, y, z;
    int grid[11][11][11];
    int * p = &grid[0][0][0];
    int good = 0;
    int bad  = 0;

    for(i = 0; i < (11*11*11); i++)
        *(p+i) = 0;

    START_M_SUB_1_MESSAGE_LOOP
        mx = m_sub_1_message->x;
        my = m_sub_1_message->y;
        mz = m_sub_1_message->z;

        if(mx >= X-BOX && mx <= X+BOX &&
            my >= Y-BOX && my <= Y+BOX &&
            mz >= Z-BOX && mz <= Z+BOX)
        {
            good++;
        }
        else bad++;

        grid[mx][my][mz]++;
    FINISH_M_SUB_1_MESSAGE_LOOP

    for(x = 0; x <= 10; x++)
    {
        for(y = 0; y <= 10; y++)
        {
            for(z = 0; z <= 10; z++)
            {
                if(grid[x][y][z] > 0) printf("%d", grid[x][y][z]);
                else printf("_");
            }
            printf(" ");
        }
        printf("\n");
    }

    printf("good=%d\tbad=%d\n", good, bad);
    BOX3D_GOOD = good;
    BOX3D_BAD = bad;

    return 0;
}

/** \fn agent_a_2()
 * \brief A description of the function
 */
int agent_a_3()
{
    int i;
    int mx, my;
    int x, y;
    int grid[11][11];
    int * p = &grid[0][0];
    int good = 0;
    int bad  = 0;
    int box = 2;

    for(i = 0; i < (11*11); i++)
        *(p+i) = 0;

    START_M_SUB_1_MESSAGE_LOOP
        mx = m_sub_1_message->x;
        my = m_sub_1_message->y;

        if(mx >= X-box && mx <= X+box &&
            my >= Y-box && my <= Y+box)
        {
            good++;
        }
        else bad++;

        grid[mx][my]++;
    FINISH_M_SUB_1_MESSAGE_LOOP

    for(x = 0; x <= 10; x++)
    {
        for(y = 0; y <= 10; y++)
        {
            if(grid[x][y] > 0) printf("%d ", grid[x][y]);
                else printf("__ ");
        }
        printf("\n");
    }

    printf("good=%d\tbad=%d\n", good, bad);
    BOX2D_GOOD = good;
    BOX2D_BAD = bad;

   return 0;
}

/** \fn agent_a_2()
 * \brief A description of the function
 */
int agent_a_4()
{

   return 0;
}

/** \fn agent_a_2()
 * \brief A description of the function
 */
int agent_a_5()
{

   return 0;
}
