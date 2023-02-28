use crate::UUID;


pub trait component_trait {
    fn new() -> Self;
    fn get_uuid(&self) -> UUID::UUID;
}

pub struct Component {
    id: UUID::UUID,
}

impl component_trait for Component{
    fn new() -> Self {
        Component { id: {UUID::UUID::new()} }
    }

    fn get_uuid(&self) -> UUID::UUID {
        self.id
    }
}
