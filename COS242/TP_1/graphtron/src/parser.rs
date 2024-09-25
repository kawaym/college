use std::fs::{self, File};
use std::io::{self, BufRead, Write};
use std::path::{Path, PathBuf};
use std::time::Instant;
use sysinfo::System;

use crate::graph::Graph;

fn read_lines_from_file<P>(filename: P) -> io::Result<io::Lines<io::BufReader<File>>>
where
    P: AsRef<Path>,
{
    let file = File::open(filename)?;
    Ok(io::BufReader::new(file).lines())
}

fn read_numbers_from_line(line: String) -> Option<(String, String)> {
    let parts: Vec<String> = line.split_whitespace().map(|s| s.to_string()).collect();

    if let (Ok(num1), Ok(num2)) = (parts[0].parse::<String>(), parts[1].parse::<String>()) {
        return Some((num1, num2));
    }

    None
}

pub fn read_graph(filename: &str) -> Graph {
    let mut graph: Graph = Graph::new(0);

    if let Ok(lines) = read_lines_from_file(filename) {
        let mut count = 0;
        for line in lines.flatten() {
            if count == 0 {
                graph = Graph::new(line.parse::<usize>().unwrap());
                count += 1;
                continue;
            }
            let numbers = read_numbers_from_line(line).unwrap();
            graph.add_edge(
                numbers.0.parse::<usize>().unwrap() - 1,
                numbers.1.parse::<usize>().unwrap() - 1,
                1,
            );
        }
    }
    return graph;
}

pub fn create_folder_and_files(filename: &str) -> std::io::Result<()> {
    let mut folder_path = PathBuf::from("./data");
    folder_path.push(filename);

    fs::create_dir_all(&folder_path)?;

    let cases: Vec<&str> = vec![
        "memory_usage.txt",
        "bfs_runtime.txt",
        "dfs_runtime.txt",
        "parent_vertex.txt",
        "connected_components.txt",
        "diameter.txt",
    ];

    for case in cases {
        let mut file_path = folder_path.clone();
        file_path.push(case);
        fs::write(file_path, "")?;
    }

    Ok(())
}

pub fn create_memory_usage_study_case(filename: &str) -> std::io::Result<()> {
    let mut sys = System::new();
    sys.refresh_memory();
    let before_loading = sys.used_memory();
    println!(
        "Memória utilizada antes do carregamento do grafo: {}",
        before_loading
    );
    let graph = read_graph(filename);
    sys.refresh_memory();
    let after_loading = sys.used_memory();
    println!("Memória utilizada após o carregamento: {}", after_loading);

    let matrix = graph.create_adjacency_matrix();
    sys.refresh_memory();
    let after_creation = sys.used_memory();
    println!(
        "Memória utilizada após a criação da matriz de adjacencia: {}",
        after_creation
    );

    println!(
        "Diferença de memória em bytes após criação da lista: {}",
        after_loading as i64 - before_loading as i64
    );
    println!(
        "Diferença de memória em bytes após criação da matriz: {}",
        after_creation as i64 - before_loading as i64
    );

    matrix;

    Ok(())
}

pub fn create_bfs_runtime_study_case(filename: &str) -> std::io::Result<()> {
    let mut graph = read_graph(filename);
    let mut results = String::new();
    let mut total_duration = std::time::Duration::new(0, 0);

    for i in 1..100 {
        let start = Instant::now();
        graph.display_bfs(i.to_string().as_str());
        let duration = start.elapsed();
        total_duration += duration;
        results.push_str(&format!("Execução {}: {:?}\n", i + 1, duration));
    }

    let average_duration = total_duration / 100;
    results.push_str(&format!(
        "Média de tempo de execução: {:?}\n",
        average_duration
    ));

    let file_path = format!("./data/grafo_2/bfs_runtime.txt");
    let mut file = fs::File::create(file_path)?;
    file.write_all(results.as_bytes())?;

    Ok(())
}
