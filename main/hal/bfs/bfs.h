///
/// @file bfs.h
/// @author Anuj Pathak (anuj@biii.in)
/// @brief
/// @version 0.1
/// @date 2024-02-17
///
/// @copyright Copyright (c) 2024
///
///
#pragma once

enum bfs_id {
    BFS_SDMMC_0,
};

struct bfs_inst {
    char *mount;
};

void bfs_mount(enum bfs_id id);

void bfs_read_file(enum bfs_id id, char *abs_path);

void bfs_write_file(enum bfs_id id, char *abs_path);
