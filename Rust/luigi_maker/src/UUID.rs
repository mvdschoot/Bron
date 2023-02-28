use std::u128;
use rand::Rng;

#[derive(Debug, Clone, Hash)]
#[derive(Eq, PartialEq)]
pub struct UUID {
    pub id: u128
}

impl UUID {
    pub fn new() -> Self {
        UUID{id: rand::thread_rng().gen::<u128>()}
    }
}