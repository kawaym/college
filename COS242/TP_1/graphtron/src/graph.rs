use std::{
    cmp::max_by,
    collections::{HashMap, VecDeque},
};

#[derive(Debug, Clone)]
struct Edge {
    target: usize,
    weight: i32,
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

    fn add_edge(&mut self, target: usize, weight: i32) {
        self.add_degree();
        self.edges.push(Edge { target, weight })
    }

    fn mark(&mut self) {
        self.status = VertexStatus::Marked
    }
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
}

impl Graph {
    pub fn new(size: usize) -> Self {
        Graph {
            vertices: vec![None; size],
            kind: GraphKind::Bidirectional,
        }
    }

    pub fn add_vertex(&mut self, id: &usize) {
        match &self.vertices[id.clone()] {
            Some(_) => (),
            None => {
                self.vertices[id.clone()] = Some(Vertex::new(id.clone()));
            }
        }
    }

    pub fn add_edge(&mut self, from: usize, to: usize, weight: i32) {
        self.add_vertex(&from.clone());
        self.add_vertex(&to.clone());
        if let Some(vertex) = &mut self.vertices[from.clone()] {
            vertex.add_edge(to.clone(), weight);
        } else {
            println!("Vertex {from} was not found!");
        }
        if let Some(vertex) = &mut self.vertices[to.clone()] {
            vertex.add_edge(from.clone(), weight);
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

    fn create_adjacency_matrix(&self) -> Vec<Vec<i32>> {
        let mut matrix = vec![vec![0; self.vertices.len()]; self.vertices.len()];

        for vertex_opt in &self.vertices {
            if let Some(vertex) = vertex_opt {
                for edge in &vertex.edges {
                    matrix[vertex.id][edge.target] = edge.weight;
                }
            }
        }

        matrix
    }

    fn display_adjacency_matrix(&self) {
        let matrix: &Vec<Vec<i32>> = &self.create_adjacency_matrix();
        let vertices_ids: Vec<usize> = self
            .vertices
            .iter()
            .filter_map(|v| v.as_ref().map(|vertex| vertex.id + 1))
            .collect();

        println!("{:?}", vertices_ids);
        println!("{:?}", matrix);
    }

    fn create_adjacency_list(&self) -> Vec<Vec<(usize, i32)>> {
        let mut list: Vec<Vec<(usize, i32)>> = Vec::new();

        for vertex_opt in &self.vertices {
            if let Some(vertex) = vertex_opt {
                let edges: Vec<(usize, i32)> = vertex
                    .edges
                    .iter()
                    .map(|edge| (edge.target.clone(), edge.weight))
                    .collect();
                list.insert(vertex.id, edges);
            }
        }

        list
    }

    fn display_adjacency_list(&self) {
        let list: Vec<Vec<(usize, i32)>> = self
            .create_adjacency_list()
            .iter()
            .map(|adjacencies: &Vec<(usize, i32)>| {
                adjacencies
                    .iter()
                    .map(|vertex| (vertex.0 + 1, vertex.1))
                    .collect::<Vec<(usize, i32)>>()
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

    fn bfs(&mut self, root_id: usize) -> Vec<usize> {
        let mut queue: VecDeque<usize> = VecDeque::new();
        let mut result: Vec<usize> = Vec::new();

        self.unmark_all_vertices();

        if let Some(_vertex) = &self.vertices[root_id] {
            queue.push_back(root_id);
            self.vertices[root_id].as_mut().unwrap().mark();

            loop {
                let vertex_id: Option<usize> = queue.pop_front();
                match vertex_id {
                    Some(id) => {
                        result.push(id.clone());

                        if let Some(vertex) = &self.vertices[id].clone() {
                            for edge in &vertex.edges {
                                match self.vertices[edge.target].as_mut().unwrap().status {
                                    VertexStatus::Marked => (),
                                    VertexStatus::Unmarked => {
                                        self.vertices[edge.target].as_mut().unwrap().mark();
                                        queue.push_back(edge.target.clone());
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

    pub fn display_bfs(&mut self, root_id: &str) {
        let parsed_id: usize = root_id.parse::<usize>().unwrap() - 1;
        let data: Vec<usize> = self.bfs(parsed_id).into_iter().map(|id| id + 1).collect();

        println!("{:?}", data);
    }

    fn dfs(&mut self, root_id: usize) -> Vec<usize> {
        let mut stack: Vec<usize> = Vec::new();
        let mut result: Vec<usize> = Vec::new();

        self.unmark_all_vertices();

        if let Some(_vertex) = &self.vertices[root_id] {
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

    pub fn display_dfs(&mut self, root_id: &str) {
        let parsed_id: usize = root_id.parse::<usize>().unwrap() - 1;
        let data: Vec<usize> = self.dfs(parsed_id).into_iter().map(|id| id + 1).collect();

        println!("{:?}", data);
    }
}
