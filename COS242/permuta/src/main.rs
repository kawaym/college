use permuta::permute;
use std::env;

fn main() {
    let args: Vec<String> = env::args().collect();
    let number = &args[1];
    let number = number.parse::<i32>().unwrap();
    let mut number_list = (1..number + 1)
        .collect::<Vec<i32>>()
        .into_iter()
        .map(|x| Some(x))
        .collect::<Vec<Option<i32>>>();

    permute(0, (number - 1).try_into().unwrap(), &mut number_list)
}
