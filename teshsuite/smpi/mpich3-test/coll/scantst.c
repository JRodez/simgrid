/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil ; -*- */
/*
 *  (C) 2001 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */
#include "mpi.h"
#include <stdio.h>
#include "mpitest.h"

static void addem(void *inv, void *inoutv, int *len, MPI_Datatype * dtype)
{
    int* invec = inv;
    int* inoutvec = inoutv;

    int i;
    for (i = 0; i < *len; i++)
        inoutvec[i] += invec[i];
}

#define BAD_ANSWER 100000

/*
    The operation is inoutvec[i] = invec[i] op inoutvec[i]
    (see 4.9.4).  The order is important.

    Note that the computation is in process rank (in the communicator)
    order, independent of the root.
 */
static void assoc(void *inv, void *inoutv, int *len, MPI_Datatype * dtype)
{
    int* invec = inv;
    int* inoutvec = inoutv;

    int i;
    for (i = 0; i < *len; i++) {
        if (inoutvec[i] <= invec[i]) {
            int rank;
            MPI_Comm_rank(MPI_COMM_WORLD, &rank);
            fprintf(stderr, "[%d] inout[0] = %d, in[0] = %d\n", rank, inoutvec[0], invec[0]);
            inoutvec[i] = BAD_ANSWER;
        }
        else
            inoutvec[i] = invec[i];
    }
}

int main(int argc, char **argv)
{
    int rank, size, i;
    int data;
    int errors = 0;
    int result = -100;
    int correct_result;
    MPI_Op op_assoc, op_addem;
    MPI_Comm comm = MPI_COMM_WORLD;

    MPI_Init(&argc, &argv);
    MPI_Op_create((MPI_User_function *) assoc, 0, &op_assoc);
    MPI_Op_create((MPI_User_function *) addem, 1, &op_addem);

    /* Run this for a variety of communicator sizes */

    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);

    data = rank;

    correct_result = 0;
    for (i = 0; i <= rank; i++)
        correct_result += i;

    MPI_Scan(&data, &result, 1, MPI_INT, MPI_SUM, comm);
    if (result != correct_result) {
        fprintf(stderr, "[%d] Error suming ints with scan\n", rank);
        errors++;
    }

    MPI_Scan(&data, &result, 1, MPI_INT, MPI_SUM, comm);
    if (result != correct_result) {
        fprintf(stderr, "[%d] Error summing ints with scan (2)\n", rank);
        errors++;
    }

    data = rank;
    result = -100;
    MPI_Scan(&data, &result, 1, MPI_INT, op_addem, comm);
    if (result != correct_result) {
        fprintf(stderr, "[%d] Error summing ints with scan (userop)\n", rank);
        errors++;
    }

    MPI_Scan(&data, &result, 1, MPI_INT, op_addem, comm);
    if (result != correct_result) {
        fprintf(stderr, "[%d] Error summing ints with scan (userop2)\n", rank);
        errors++;
    }
    result = -100;
    data = rank;
    MPI_Scan(&data, &result, 1, MPI_INT, op_assoc, comm);
    if (result == BAD_ANSWER) {
        fprintf(stderr, "[%d] Error scanning with non-commutative op\n", rank);
        errors++;
    }

    MPI_Op_free(&op_assoc);
    MPI_Op_free(&op_addem);

    MPI_Finalize();
    if (errors)
        printf("[%d] done with ERRORS(%d)!\n", rank, errors);
    else {
        if (rank == 0)
            printf(" No Errors\n");
    }

    return errors;
}
