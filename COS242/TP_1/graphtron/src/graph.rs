use core::f64;
use ordered_float::OrderedFloat;
use std::{
    cmp::Ordering,
    collections::{BinaryHeap, VecDeque},
    process::exit,
    usize,
};

#[derive(Debug, Clone, Eq, PartialEq)]
struct State {
    cost: OrderedFloat<f64>,
    position: usize,
}

impl Ord for State {
    fn cmp(&self, other: &Self) -> Ordering {
        other.cost.cmp(&self.cost) // inversão para que o BinaryHeap funcione como uma min-heap
    }
}

impl PartialOrd for State {
    fn partial_cmp(&self, other: &Self) -> Option<Ordering> {
        Some(self.cmp(other))
    }
}

#[derive(Debug, Clone)]
pub struct Edge {
    target: usize,
    weight: f64,
}

#[derive(Debug, Clone)]
enum VertexStatus {
    Marked,
    Unmarked,
}

#[derive(Debug, Clone)]
pub struct Vertex {
    id: usize,
    edges: Vec<Edge>,
    degree: u32,
    status: VertexStatus,
}

impl Vertex {
    fn new(id: usize) -> Self {
        Vertex {
            id,
            edges: Vec::new(),
            degree: 0,
            status: VertexStatus::Unmarked,
        }
    }

    fn add_degree(&mut self) {
        self.degree = self.degree + 1
    }

    fn add_edge(&mut self, target: usize, weight: f64) {
        self.add_degree();
        self.edges.push(Edge { target, weight })
    }

    fn mark(&mut self) {
        self.status = VertexStatus::Marked
    }
}

#[derive(Debug)]
struct VertexTreeView {
    id: usize,
    level: usize,
    parent: usize,
}

#[derive(Debug)]
enum GraphKind {
    Bidirectional,
    Unidirectional,
}

#[derive(Debug)]
pub struct Graph {
    vertices: Vec<Option<Vertex>>,
    kind: GraphKind,
    has_negative_weight: bool,
}

impl Graph {
    pub fn new(size: usize) -> Self {
        Graph {
            vertices: vec![None; size],
            kind: GraphKind::Bidirectional,
            has_negative_weight: false,
        }
    }

    pub fn has_negative_weight(&self) -> bool {
        self.has_negative_weight
    }

    pub fn add_vertex(&mut self, id: &usize) {
        match &self.vertices[id.clone()] {
            Some(_) => (),
            None => {
                self.vertices[id.clone()] = Some(Vertex::new(id.clone()));
            }
        }
    }

    pub fn add_edge(&mut self, from: usize, to: usize, weight: f64) {
        if weight < 0.0 {
            self.has_negative_weight = true;
        }
        self.add_vertex(&from.clone());
        self.add_vertex(&to.clone());
        if let Some(vertex) = &mut self.vertices[from.clone()] {
            vertex.add_edge(to.clone(), weight);
            vertex.add_degree();
        } else {
            println!("Vertex {from} was not found!");
        }
        if let Some(vertex) = &mut self.vertices[to.clone()] {
            vertex.add_edge(from.clone(), weight);
            vertex.add_degree();
        } else {
            println!("Vertex {to} was not found!");
        }
    }

    pub fn display(&self, format: String) {
        if format == "Matriz" {
            self.display_adjacency_matrix();
            return;
        }

        if format == "Lista" {
            self.display_adjacency_list();
            return;
        }

        for vertex in &self.vertices {
            match vertex {
                Some(info) => {
                    println!("{}", info.id + 1);
                    for edge in &info.edges {
                        println!(" -> {} (weight {})", edge.target + 1, edge.weight)
                    }
                }
                None => (),
            }
        }
    }

    pub fn get_vertices_number(&self) -> u32 {
        self.vertices.len() as u32
    }

    pub fn get_edges_number(&self) -> u32 {
        let mut number = 0;
        for vertex in &self.vertices {
            match vertex {
                Some(info) => number += info.degree,
                None => (),
            }
        }

        match self.kind {
            GraphKind::Bidirectional => number = number / 2,
            GraphKind::Unidirectional => number = number,
        }

        return number;
    }

    pub fn get_degree_minimum(&self) -> u32 {
        let mut number: u32 = u32::max_value();
        for vertex_opt in &self.vertices {
            if let Some(vertex) = vertex_opt {
                if vertex.degree < number {
                    number = vertex.degree
                }
            }
        }
        number
    }

    pub fn get_degree_maximum(&self) -> u32 {
        let mut number: u32 = 0;
        for vertex_opt in &self.vertices {
            if let Some(vertex) = vertex_opt {
                if vertex.degree > number {
                    number = vertex.degree
                }
            }
        }
        number
    }

    pub fn get_degree_average(&self) -> f32 {
        let mut number = 0.0;
        for vertex_opt in &self.vertices {
            if let Some(vertex) = vertex_opt {
                number += vertex.degree as f32
            }
        }
        number /= self.vertices.len() as f32;
        number
    }

    pub fn get_degree_median(&self) -> f32 {
        let mut vertices: Vec<u32> = vec![];
        let mut number: f32 = -1.0;
        for vertex in &self.vertices {
            match vertex {
                Some(info) => {
                    vertices.push(info.degree.clone());
                }
                None => (),
            }
        }
        vertices.sort_by(|a, b| a.partial_cmp(b).unwrap());

        if vertices.len() % 2 != 0 {
            number = vertices[vertices.len() / 2] as f32
        } else {
            number = (vertices[vertices.len() / 2] as f32
                + vertices[vertices.len() / 2 - 1] as f32)
                / 2.0;
        }
        number
    }

    pub fn create_adjacency_matrix(&self) -> Vec<Vec<f64>> {
        let mut matrix = vec![vec![0.0; self.vertices.len()]; self.vertices.len()];

        for vertex_opt in &self.vertices {
            if let Some(vertex) = vertex_opt {
                for edge in &vertex.edges {
                    matrix[vertex.id][edge.target] = edge.weight;
                }
            }
        }

        matrix
    }

    pub fn display_adjacency_matrix(&self) {
        let matrix: &Vec<Vec<f64>> = &self.create_adjacency_matrix();
        let vertices_ids: Vec<usize> = self
            .vertices
            .iter()
            .filter_map(|v| v.as_ref().map(|vertex| vertex.id + 1))
            .collect();

        println!("{:?}", vertices_ids);
        println!("{:?}", matrix);
    }

    pub fn create_adjacency_list(&self) -> Vec<Vec<(usize, f64)>> {
        let mut list: Vec<Vec<(usize, f64)>> = Vec::new();

        for vertex_opt in &self.vertices {
            if let Some(vertex) = vertex_opt {
                let edges: Vec<(usize, f64)> = vertex
                    .edges
                    .iter()
                    .map(|edge| (edge.target.clone(), edge.weight))
                    .collect();
                list.insert(vertex.id, edges);
            }
        }

        list
    }

    pub fn display_adjacency_list(&self) {
        let list: Vec<Vec<(usize, f64)>> = self
            .create_adjacency_list()
            .iter()
            .map(|adjacencies: &Vec<(usize, f64)>| {
                adjacencies
                    .iter()
                    .map(|vertex| (vertex.0 + 1, vertex.1))
                    .collect::<Vec<(usize, f64)>>()
            })
            .collect();
        let vertices_ids: Vec<usize> = self
            .vertices
            .iter()
            .filter_map(|v| v.as_ref().map(|vertex| vertex.id + 1))
            .collect();

        println!("{:?}", vertices_ids);

        println!("{:?}", list)
    }

    fn unmark_all_vertices(&mut self) {
        for vertex_opt in &mut self.vertices {
            if let Some(vertex) = vertex_opt {
                vertex.status = VertexStatus::Unmarked
            }
        }
    }

    fn create_tree_view(&self, vector: Vec<(usize, usize, Option<usize>)>) -> Vec<VertexTreeView> {
        let mut result: Vec<VertexTreeView> = Vec::new();

        for vertex in vector {
            result.push(VertexTreeView {
                id: vertex.0 + 1,
                level: vertex.1,
                parent: match vertex.2 {
                    Some(id) => id + 1,
                    None => 0,
                },
            });
        }

        result
    }

    pub fn bfs(
        &mut self,
        root_id: usize,
        stop_id: Option<usize>,
    ) -> Vec<(usize, usize, Option<usize>)> {
        let mut queue: VecDeque<usize> = VecDeque::new();
        let mut result: Vec<usize> = Vec::new();
        let mut tree: Vec<(usize, usize, Option<usize>)> = Vec::new();

        let vertices_len = self.get_vertices_number();
        let mut levels: Vec<usize> = vec![0; vertices_len as usize];
        let mut parents: Vec<Option<usize>> = vec![None; vertices_len as usize];

        let mut stop_flag = false;

        self.unmark_all_vertices();

        if let Some(_vertex) = &self.vertices[root_id] {
            queue.push_back(root_id);
            self.vertices[root_id].as_mut().unwrap().mark();

            loop {
                let vertex_id: Option<usize> = queue.pop_front();
                match vertex_id {
                    Some(id) => {
                        result.push(id.clone());

                        if let Some(parent) = parents[id] {
                            levels[id] = levels[parent] + 1
                        }

                        if stop_flag {
                            break;
                        }

                        if let Some(vertex) = &self.vertices[id].clone() {
                            for edge in &vertex.edges {
                                match self.vertices[edge.target].as_mut().unwrap().status {
                                    VertexStatus::Marked => (),
                                    VertexStatus::Unmarked => {
                                        parents[edge.target] = Some(id);
                                        self.vertices[edge.target].as_mut().unwrap().mark();
                                        queue.push_back(edge.target.clone());
                                    }
                                }
                                if let Some(stop_id_parsed) = stop_id {
                                    if stop_id_parsed == edge.target {
                                        stop_flag = true;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                    None => break,
                }
            }
        }

        for i in 0..levels.len() {
            tree.push((i, levels[i], parents[i]));
        }
        tree
    }

    pub fn display_bfs(&mut self, root_id: &str) {
        let parsed_root_id: usize = root_id.parse::<usize>().unwrap() - 1;
        let data = self.bfs(parsed_root_id, None);

        let tree = self.create_tree_view(data);
        for vertex in tree {
            // println!("vértice: {}", vertex.id);
            // println!("  -> nível: {}", vertex.level);
            // println!("  -> pai: {}", vertex.parent);
        }
    }

    pub fn calculate_distance(&mut self, start_id: usize, end_id: usize) -> usize {
        let data = self.bfs(start_id, Some(end_id));
        let mut distance = usize::MIN;

        for vector in data {
            if vector.1 > distance {
                distance = vector.1;
            }
        }

        distance
    }

    pub fn display_distance(&mut self, start_id: &str, end_id: &str) -> usize {
        let parsed_start_id = start_id.parse::<usize>().unwrap() - 1;
        let parsed_end_id = end_id.parse::<usize>().unwrap() - 1;

        let data = self.calculate_distance(parsed_start_id, parsed_end_id);

        println!(
            "Distância entre os vértices {} e {} é de: {}",
            start_id, end_id, data
        );

        data
    }

    pub fn calculate_diameter(&mut self) -> usize {
        let mut diameter = usize::MIN;

        for vertex_start_opt in &self.vertices.clone() {
            if let Some(vertex_start) = vertex_start_opt {
                for vertex_end_opt in &self.vertices.clone() {
                    if let Some(vertex_end) = vertex_end_opt {
                        if vertex_start.id == vertex_end.id {
                            continue;
                        }

                        let distance = &mut self.calculate_distance(vertex_start.id, vertex_end.id);
                        if *distance > diameter {
                            diameter = *distance;
                        }
                    }
                }
            }
        }
        diameter
    }

    pub fn calculate_approximate_diameter(&mut self) -> usize {
        let mut diameter = usize::MIN;
        let mut chosen_vertex: usize = 0;
        let vertex_count = self.get_vertices_number() as usize;

        for vertex_idx in 0..vertex_count {
            if let Some(vertex) = &self.vertices[vertex_idx] {
                let bfs = self.bfs(vertex.id, None);
                for i in bfs {
                    if i.1 > diameter {
                        diameter = i.1;
                        chosen_vertex = i.0;
                    }
                }
            }
        }

        let bfs = self.bfs(chosen_vertex, None);
        for i in bfs {
            if i.1 > diameter {
                diameter = i.1;
            }
        }

        diameter
    }

    pub fn dfs(&mut self, root_id: usize) -> Vec<(usize, usize, Option<usize>)> {
        let mut stack: Vec<usize> = Vec::new();
        let mut result: Vec<usize> = Vec::new();
        let mut tree: Vec<(usize, usize, Option<usize>)> = Vec::new();

        let vertices_len = self.get_vertices_number();
        let mut levels: Vec<usize> = vec![0; vertices_len as usize];
        let mut parents: Vec<Option<usize>> = vec![None; vertices_len as usize];

        self.unmark_all_vertices();

        if let Some(_vertex) = &self.vertices[root_id] {
            stack.push(root_id);
            self.vertices[root_id].as_mut().unwrap().mark();

            loop {
                let vertex_id: Option<usize> = stack.pop();
                match vertex_id {
                    Some(id) => {
                        result.push(id.clone());

                        if let Some(parent) = parents[id] {
                            levels[id] = levels[parent] + 1
                        }

                        if let Some(vertex) = &self.vertices[id].clone() {
                            for edge in &vertex.edges {
                                match self.vertices[edge.target].as_mut().unwrap().status {
                                    VertexStatus::Marked => (),
                                    VertexStatus::Unmarked => {
                                        parents[edge.target] = Some(id);
                                        self.vertices[edge.target].as_mut().unwrap().mark();
                                        stack.push(edge.target.clone());
                                    }
                                }
                            }
                        }
                    }
                    None => break,
                }
            }
        }

        for i in 0..levels.len() {
            tree.push((i, levels[i], parents[i]));
        }

        tree
    }

    pub fn display_dfs(&mut self, root_id: &str) {
        let parsed_id: usize = root_id.parse::<usize>().unwrap() - 1;
        let data = self.dfs(parsed_id);

        let tree = self.create_tree_view(data);
        for vertex in tree {
            // println!("vértice: {}", vertex.id);
            // println!("  -> nível: {}", vertex.level);
            // println!("  -> pai: {}", vertex.parent);
        }
    }

    fn dfs_connected_components(&mut self, root_id: usize) -> Vec<usize> {
        let mut stack: Vec<usize> = Vec::new();
        let mut result: Vec<usize> = Vec::new();

        if let Some(vertex) = &self.vertices[root_id] {
            stack.push(root_id);
            self.vertices[root_id].as_mut().unwrap().mark();

            loop {
                let vertex_id: Option<usize> = stack.pop();
                match vertex_id {
                    Some(id) => {
                        result.push(id.clone());

                        if let Some(vertex) = &self.vertices[id].clone() {
                            for edge in &vertex.edges {
                                match self.vertices[edge.target].as_mut().unwrap().status {
                                    VertexStatus::Marked => (),
                                    VertexStatus::Unmarked => {
                                        self.vertices[edge.target].as_mut().unwrap().mark();
                                        stack.push(edge.target.clone());
                                    }
                                }
                            }
                        }
                    }
                    None => break,
                }
            }
        }
        result
    }

    pub fn calculate_connected_components(&mut self) -> Vec<Vec<usize>> {
        let mut components: Vec<Vec<usize>> = Vec::new();
        let vertex_count = self.get_vertices_number() as usize;

        for vertex_idx in 0..vertex_count {
            if let Some(vertex) = &self.vertices[vertex_idx] {
                match vertex.status {
                    VertexStatus::Marked => (),
                    VertexStatus::Unmarked => {
                        let component = self.dfs_connected_components(vertex.id);
                        components.push(component.clone())
                    }
                }
            }
        }
        components
    }

    pub fn display_components_info(&mut self) {
        let mut components = self.calculate_connected_components();
        components.sort_by(|a, b| a.len().cmp(&b.len()));

        components.iter_mut().for_each(|vector| {
            vector.iter_mut().for_each(|vertex| *vertex += 1);
            vector.sort();
        });

        println!("O grafo possui {} componentes conexas", components.len());
        println!(
            "A menor componente conexa do grafo é: {:?}",
            components.first().unwrap()
        );
        println!(
            "A maior componente conexa do grafo é: {:?}",
            components.last().unwrap()
        )
    }

    pub fn create_dijkstra_vector(
        &self,
        root_id: usize,
        destinations: &[usize],
    ) -> (Vec<f64>, Vec<Vec<Edge>>) {
        if self.has_negative_weight() {
            println!(
                "This library does not works with negative cycles for minimum distances computing"
            );
            exit(1)
        }

        let n = self.vertices.len();
        let mut distances = vec![f64::INFINITY; n];
        let mut visited = vec![false; n];
        let mut parents = vec![None; n];

        distances[root_id] = 0.0;

        for _ in 0..n {
            let mut u = None;
            for i in 0..n {
                if !visited[i] && (u.is_none() || distances[i] < distances[u.unwrap()]) {
                    u = Some(i);
                }
            }

            let u = match u {
                Some(v) => v,
                None => break,
            };

            visited[u] = true;

            if let Some(vertex) = &self.vertices[u] {
                for edge in &vertex.edges {
                    let new_distance = distances[u] + edge.weight;
                    if new_distance < distances[edge.target] {
                        distances[edge.target] = new_distance;
                        parents[edge.target] = Some(u);
                    }
                }
            }
        }

        let mut paths = Vec::new();
        for &target in destinations {
            let mut path = Vec::new();
            let mut current = target;

            // Segue o caminho de `target` até `root_id` usando o vetor `parents`
            while let Some(parent) = parents[current] {
                let weight = distances[current] - distances[parent];
                path.push(Edge {
                    target: current,
                    weight,
                });
                current = parent;
            }

            path.reverse(); // Inverte o caminho para que vá do `root_id` ao `target`
            paths.push(path);
        }

        (distances, paths)
    }

    pub fn create_dijkstra_heap(
        &self,
        root_id: usize,
        destinations: &[usize],
    ) -> (Vec<f64>, Vec<Vec<Edge>>) {
        let n = self.vertices.len();
        let mut distances = vec![OrderedFloat(f64::INFINITY); n];
        let mut heap = BinaryHeap::new();
        let mut parents: Vec<Option<usize>> = vec![None; n];

        distances[root_id] = OrderedFloat(0.0);
        heap.push(State {
            cost: OrderedFloat(0.0),
            position: root_id,
        });

        while let Some(State { cost, position }) = heap.pop() {
            if cost > distances[position] {
                continue;
            }

            if let Some(vertex) = &self.vertices[position] {
                for edge in &vertex.edges {
                    let next_cost = cost + OrderedFloat(edge.weight);

                    if next_cost < distances[edge.target] {
                        distances[edge.target] = next_cost;
                        parents[edge.target] = Some(position);
                        heap.push(State {
                            cost: next_cost,
                            position: edge.target,
                        });
                    }
                }
            }
        }

        // Converte `distances` de OrderedFloat<f64> para f64 puro
        let distances: Vec<f64> = distances.into_iter().map(|d| d.into_inner()).collect();

        // Construção dos caminhos mínimos para os vértices de `destinations`
        let mut paths = Vec::new();
        for &target in destinations {
            let mut path = Vec::new();
            let mut current = target;

            // Segue o caminho de `target` até `root_id` usando o vetor `parents`
            while let Some(parent) = parents[current] {
                let weight = distances[current] - distances[parent];
                path.push(Edge {
                    target: current,
                    weight,
                });
                current = parent;
            }

            path.reverse(); // Inverte o caminho para que vá do `root_id` ao `target`
            paths.push(path);
        }

        (distances, paths)
    }

    pub fn display_dijkstra_vector(&self, root_id: &str) {
        let vertex = root_id.parse::<usize>().unwrap() - 1;
        let distance = self.create_dijkstra_vector(vertex, &[]).0;

        println!("O vetor de distâncias para o grafo é:\n{:?}", distance);
    }

    pub fn display_dijkstra_heap(&self, root_id: &str) {
        let vertex = root_id.parse::<usize>().unwrap() - 1;
        let distance = self.create_dijkstra_heap(vertex, &[]).0;

        println!("O heap de distâncias para o grafo é:\n{:?}", distance);
    }
}
