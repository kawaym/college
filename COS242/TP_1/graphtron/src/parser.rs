use std::fs::File;
use std::io::{self, BufRead};
use std::path::Path;

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
