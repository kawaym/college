use std::fs::{self, File};
use std::io::{self, BufRead, Write};
use std::path::{Path, PathBuf};
use std::time::Instant;
use sysinfo::System;

use crate::graph::{Edge, Graph};

fn read_lines_from_file<P>(filename: P) -> io::Result<io::Lines<io::BufReader<File>>>
where
    P: AsRef<Path>,
{
    let file = File::open(filename)?;
    Ok(io::BufReader::new(file).lines())
}

fn read_numbers_from_line(line: String) -> Option<(String, String, String)> {
    let parts: Vec<String> = line.split_whitespace().map(|s| s.to_string()).collect();

    if let (Ok(num1), Ok(num2), Ok(num3)) = (
        parts[0].parse::<String>(),
        parts[1].parse::<String>(),
        parts[2].parse::<String>(),
    ) {
        return Some((num1, num2, num3));
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
                numbers.2.parse::<f64>().unwrap(),
            );
        }
    }
    return graph;
}

pub fn create_study_cases() -> std::io::Result<()> {
    let graphs = [
        "grafo_W_1",
        "grafo_W_2",
        "grafo_W_3",
        "grafo_W_4",
        "grafo_W_5",
    ];

    for filename in graphs {
        create_folder_and_files(filename)?;
        create_weighted_distances_study_case(filename)?;
        // create_dijkstra_comparison_study_case(filename)?;
        // create_memory_usage_study_case(filename)?;
        // create_bfs_runtime_study_case(filename)?;
        // create_dfs_runtime_study_case(filename)?;
        // create_parent_study_case(filename)?;
        // create_distance_study_case(filename)?;
        // create_connected_components_study_case(filename)?;
    }

    // for filename in graphs {
    //     create_diameter_study_case(filename)?;
    // }

    Ok(())
}

pub fn create_folder_and_files(filename: &str) -> std::io::Result<()> {
    let mut folder_path = PathBuf::from("./data");
    folder_path.push(filename);

    fs::create_dir_all(&folder_path)?;

    let cases: Vec<&str> = vec![
        // "memory_usage.txt",
        // "bfs_runtime.txt",
        // "dfs_runtime.txt",
        // "parent_vertex.txt",
        // "connected_components.txt",
        // "diameter.txt",
        "distances.txt",
        "dijkstra_runtime.txt",
    ];

    for case in cases {
        let mut file_path = folder_path.clone();
        file_path.push(case);
        fs::write(file_path, "")?;
    }

    Ok(())
}

pub fn create_memory_usage_study_case(filename: &str) -> std::io::Result<()> {
    let mut results = String::new();
    let mut sys = System::new();
    sys.refresh_memory();
    let before_loading = sys.used_memory();
    results += format!(
        "Memória utilizada antes do carregamento do grafo: {}\n",
        before_loading
    )
    .as_str();
    let graph = read_graph(format!("./data/{}.txt", filename).as_str());
    sys.refresh_memory();
    let after_loading = sys.used_memory();
    results += format!("Memória utilizada após o carregamento: {}\n", after_loading).as_str();

    results += format!(
        "Diferença de memória em bytes após criação da lista: {}\n",
        after_loading as i64 - before_loading as i64
    )
    .as_str();

    drop(graph);

    let file_path = format!("./data/{filename}/memory_usage.txt");
    let mut file = fs::File::create(file_path)?;
    file.write_all(results.as_bytes())?;

    Ok(())
}

pub fn create_bfs_runtime_study_case(filename: &str) -> std::io::Result<()> {
    let mut graph = read_graph(format!("./data/{}.txt", filename).as_str());
    let mut results = String::new();
    let mut total_duration = std::time::Duration::new(0, 0);

    for i in 1..101 {
        let start = Instant::now();
        graph.display_bfs(i.to_string().as_str());
        let duration = start.elapsed();
        total_duration += duration;
        results += format!("Execução {}: {:?}\n", i, duration).as_str();
    }

    let average_duration = total_duration / 100;
    results += format!("Média de tempo de execução: {:?}\n", average_duration).as_str();

    let file_path = format!("./data/{filename}/bfs_runtime.txt");
    let mut file = fs::File::create(file_path)?;
    file.write_all(results.as_bytes())?;

    Ok(())
}

pub fn create_dfs_runtime_study_case(filename: &str) -> std::io::Result<()> {
    let mut graph = read_graph(format!("./data/{}.txt", filename).as_str());
    let mut results = String::new();
    let mut total_duration = std::time::Duration::new(0, 0);

    for i in 1..101 {
        let start = Instant::now();
        graph.display_dfs(i.to_string().as_str());
        let duration = start.elapsed();
        total_duration += duration;
        results += format!("Execução {}: {:?}\n", i, duration).as_str();
    }

    let average_duration = total_duration / 100;
    results += format!("Média de tempo de execução: {:?}\n", average_duration).as_str();

    let file_path = format!("./data/{filename}/dfs_runtime.txt");
    let mut file = fs::File::create(file_path)?;
    file.write_all(results.as_bytes())?;

    Ok(())
}

pub fn create_parent_study_case(filename: &str) -> std::io::Result<()> {
    let mut graph = read_graph(format!("./data/{}.txt", filename).as_str());
    let mut results = String::new();

    for root_id in 0..3 {
        let result_bfs = graph.bfs(root_id, None);
        for target in [9, 19, 29] {
            results += format!(
                "O pai do vértice {} na árvore BFS iniciando a partir do vértice {} é: {}\n",
                target + 1,
                root_id + 1,
                result_bfs[target].2.unwrap_or(0)
            )
            .as_str()
        }
        let result_dfs = graph.dfs(root_id);
        for target in [9, 19, 29] {
            results += format!(
                "O pai do vértice {} na árvore DFS iniciando a partir do vértice {} é: {}\n",
                target + 1,
                root_id + 1,
                result_dfs[target].2.unwrap_or(0)
            )
            .as_str();
        }
        results += "\n\n";
    }

    let file_path = format!("./data/{filename}/parent_vertex.txt");
    let mut file = fs::File::create(file_path)?;
    file.write_all(results.as_bytes())?;

    Ok(())
}

pub fn create_distance_study_case(filename: &str) -> std::io::Result<()> {
    let mut graph = read_graph(format!("./data/{}.txt", filename).as_str());
    let mut results = String::new();

    for (start_id, end_id) in [(9, 19), (9, 29), (19, 29)] {
        let result = graph.calculate_distance(start_id, end_id);

        results += format!(
            "A distância entre {} e {} é de: {}\n",
            start_id + 1,
            end_id + 1,
            result
        )
        .as_str()
    }

    let file_path = format!("./data/{filename}/distance.txt");
    let mut file = fs::File::create(file_path)?;
    file.write_all(results.as_bytes())?;

    Ok(())
}

pub fn create_connected_components_study_case(filename: &str) -> std::io::Result<()> {
    let mut graph = read_graph(format!("./data/{}.txt", filename).as_str());
    let mut results = String::new();

    let mut result = graph.calculate_connected_components();

    result.sort_by(|a, b| a.len().cmp(&b.len()));

    result.iter_mut().for_each(|vector| {
        vector.iter_mut().for_each(|vertex| *vertex += 1);
        vector.sort();
    });

    results += format!("Número de componentes conexas: {}\nTamanho da maior componente conexa: {:?}\nTamanho da menor componente conexa: {:?}\n\nComponentes conexas: {:?}", result.len(), result.last().unwrap().len(), result.first().unwrap().len(), result).as_str();

    let file_path = format!("./data/{filename}/connected_components.txt");
    let mut file = fs::File::create(file_path)?;
    file.write_all(results.as_bytes())?;

    Ok(())
}

pub fn create_diameter_study_case(filename: &str) -> std::io::Result<()> {
    let mut graph = read_graph(format!("./data/{}.txt", filename).as_str());
    let mut results = String::new();

    let result = graph.calculate_approximate_diameter();
    results += format!("O diâmetro do grafo é: {}", result).as_str();

    let file_path = format!("./data/{filename}/diameter.txt");
    let mut file = fs::File::create(file_path)?;
    file.write_all(results.as_bytes())?;

    Ok(())
}

pub fn create_weighted_distances_study_case(filename: &str) -> std::io::Result<()> {
    let graph = read_graph(format!("./data/{}.txt", filename).as_str());
    let mut results = String::new();

    let (distances, trees) = graph.create_dijkstra_heap(9, &[19, 29, 39, 49, 59]);

    let mut target_distances: Vec<f64> = vec![];
    for i in [19, 29, 39, 49, 59] {
        target_distances.push(distances[i]);
    }

    results += format!("As distâncias computadas são: {:?}\n", target_distances).as_str();
    results += format!("Os caminhos mínimos computados são: {:?}", trees).as_str();

    let file_path = format!("./data/{filename}/distances.txt");
    let mut file = fs::File::create(file_path)?;
    file.write_all(results.as_bytes())?;

    Ok(())
}

pub fn create_dijkstra_comparison_study_case(filename: &str) -> std::io::Result<()> {
    let mut graph = read_graph(format!("./data/{}.txt", filename).as_str());
    let mut results = String::new();
    let mut total_duration = std::time::Duration::new(0, 0);

    for i in 0..100 {
        let start = Instant::now();
        graph.create_dijkstra_heap(i, &[]);
        let duration = start.elapsed();
        total_duration += duration;
        results += format!("Execução {}: {:?}\n", i, duration).as_str();
    }

    let average_duration = total_duration / 100;
    results += format!(
        "Média de tempo de execução (HEAP): {:?}\n",
        average_duration
    )
    .as_str();

    let mut total_duration = std::time::Duration::new(0, 0);

    for i in 0..100 {
        let start = Instant::now();
        graph.create_dijkstra_vector(i, &[]);
        let duration = start.elapsed();
        total_duration += duration;
        results += format!("Execução {}: {:?}\n", i, duration).as_str();
    }

    let average_duration = total_duration / 100;
    results += format!(
        "Média de tempo de execução (VECTOR): {:?}\n",
        average_duration
    )
    .as_str();

    let file_path = format!("./data/{filename}/dijkstra_runtime.txt");
    let mut file = fs::File::create(file_path)?;
    file.write_all(results.as_bytes())?;

    Ok(())
}
