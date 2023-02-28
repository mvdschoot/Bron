use std::{collections::HashMap, any::TypeId};

use crate::{UUID, Components};

#[derive(Debug)]
pub struct NoSuchComponentException {} 

pub trait Entity {
    fn get_component<T: Components::component_trait> (&self) -> Result<&UUID::UUID, NoSuchComponentException>;
    fn get_components(&self, types: &[TypeId]) -> Result<Vec<&UUID::UUID>, NoSuchComponentException>;
    fn contains<T: Components::component_trait>(&self) -> bool;
    fn contains_all(&self, types: &[TypeId]) -> bool;
    fn added_component<T: Components::component_trait>(&mut self, ) -> ;
}

#[derive(Debug)]
pub struct BasicEntity {
    pub id: UUID::UUID,
    pub name: String,
    components: HashMap<TypeId, UUID::UUID>
}

impl BasicEntity {
    fn new(entity_name: String) -> Self {
        BasicEntity {
            id: UUID::UUID::new(),
            name: entity_name,
            components: HashMap::new(),
        }
    }

    fn get_component_by_type(&self, type_id: TypeId) -> Result<&UUID::UUID, NoSuchComponentException> {
        self.components.get(&type_id).ok_or(NoSuchComponentException {  })
    }
}

impl Entity for BasicEntity {

    fn get_component<T: Components::component_trait> (&self) -> Result<&UUID::UUID, NoSuchComponentException> {
        self.components.get(&TypeId::of::<T>()).ok_or(NoSuchComponentException {})
    }

    fn get_components(&self, types: &[TypeId]) -> Result<Vec<&UUID::UUID>, NoSuchComponentException> {
        let mut res: Vec<&UUID::UUID> = Vec::new();
        for t in types {
            res.push(&*self.get_component_by_type(*t)?);
        }
        Ok(res)
    }

    fn contains<T: Components::component_trait>(&self) -> bool {
        self.components.contains_key(&TypeId::of::<T>())
    }

    fn contains_all(&self, types: &[TypeId]) -> bool {
        for t in types {
            if !self.components.contains_key(t) {
                return false
            }
        }
        true
    }

    // pub fn add_component(&self, )
}