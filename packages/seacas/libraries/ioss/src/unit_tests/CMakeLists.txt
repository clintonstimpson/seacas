
TRIBITS_ADD_EXECUTABLE(
 Utst_textmesh
 SOURCES unitMain.C UnitTestTextMesh.C
)

TRIBITS_ADD_EXECUTABLE(
 Utst_blockbatchread
 SOURCES unitMain.C UnitTestElementBlockBatchRead.C
)

TRIBITS_ADD_TEST(
        Utst_blockbatchread
        NAME Utst_blockbatchread
        NUM_MPI_PROCS 2
)

TRIBITS_ADD_TEST(
        Utst_blockbatchread
        NAME Utst_blockbatchread
        NUM_MPI_PROCS 4
)

TRIBITS_ADD_TEST(
        Utst_blockbatchread
        NAME Utst_blockbatchread
        NUM_MPI_PROCS 8
)

TRIBITS_ADD_TEST(
        Utst_textmesh
        NAME Utst_textmesh
        NUM_MPI_PROCS 1
)

TRIBITS_ADD_TEST(
        Utst_textmesh
        NAME Utst_textmesh
        NUM_MPI_PROCS 2
)

TRIBITS_ADD_TEST(
        Utst_textmesh
        NAME Utst_textmesh
        NUM_MPI_PROCS 3
)

TRIBITS_ADD_TEST(
        Utst_textmesh
        NAME Utst_textmesh
        NUM_MPI_PROCS 4
)

TRIBITS_INCLUDE_DIRECTORIES(
  "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_CURRENT_SOURCE_DIR}/../"
  "${CMAKE_CURRENT_BINARY_DIR}/../"
)
