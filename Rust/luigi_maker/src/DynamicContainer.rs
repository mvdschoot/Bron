use std::{
    alloc::{alloc, realloc, Layout, dealloc},
    collections::HashMap, any::TypeId, 
};
use crate::{UUID::UUID, Components::component_trait, ComponentHandle};

pub struct DynamicContainer {
    storage: *mut u8,
    storage_free_ptr: *mut u8,
    storage_size: usize,
    layout: Layout,
    storage_content: HashMap<UUID, (*mut u8, usize)>,
    frag_threshold: f32,
    frag_hole_size: usize,
}


impl Drop for DynamicContainer {
    fn drop(&mut self) {
        unsafe{ dealloc(self.storage, self.layout); }
    }
}

impl DynamicContainer{
    pub fn new(initial_size: usize, align: usize, hole_threshold: f32) -> Self {
        // Check if alignment is power of 2
        assert!((align & (align - 1)) == 0);
        let layout = Layout::from_size_align(initial_size, align).unwrap();
        let storage = unsafe { alloc(layout) };
        DynamicContainer {
            storage,
            storage_free_ptr: storage,
            storage_size: initial_size,
            layout,
            storage_content: HashMap::new(),
            frag_threshold: hole_threshold,
            frag_hole_size: 0,
        }
    }

    pub fn insert<T>(&mut self, uuid: UUID, element: T) -> &mut T {
        let size = std::mem::size_of::<T>();
        self.resizeIfNeeded(size as isize);

        let ptr = self.storage_free_ptr as *mut T;
        unsafe{ ptr.write(element); }

        self.storage_content.insert(uuid, (ptr as *mut u8, size));
        unsafe{ self.storage_free_ptr = self.storage_free_ptr.offset(size as isize) as *mut u8; }

        unsafe {&mut *ptr}
    }

    pub fn insert_no_uuid<T: component_trait>(&mut self, element: T) -> &mut T {
        let size = std::mem::size_of::<T>();
        self.resizeIfNeeded(size as isize);

        let ptr = self.storage_free_ptr as *mut T;
        unsafe{ ptr.write(element); }

        let uuid = element.get_uuid();
        self.storage_content.insert(uuid, (ptr as *mut u8, size));
        unsafe { self.storage_free_ptr = self.storage_free_ptr.offset(size as isize); }

        unsafe{ &mut *ptr }
    }

    pub fn remove<T>(&mut self, element: UUID) -> T {
        let (ptr, size) = self.storage_content.remove(&element).unwrap();

        self.frag_hole_size += size;

        unsafe {
            std::ptr::read(ptr as *const T)
        }
    }

    pub fn remove_no_return(&mut self, element: UUID) {
        let (ptr, size) = self.storage_content.remove(&element).unwrap();

        unsafe{
            if ptr.offset(size as isize) == self.storage_free_ptr {
                self.storage_free_ptr = self.storage_free_ptr.sub(size);
            } else {
                self.frag_hole_size += size;
            }
        }
    }

    pub fn insert_external(&mut self, uuid: UUID, size: usize) -> *mut u8 {
        self.resizeIfNeeded(size as isize);

        let ptr = self.storage_free_ptr;
        self.storage_content.insert(uuid, (ptr, size));
        unsafe { self.storage_free_ptr = self.storage_free_ptr.offset(size as isize); }

        ptr
    }

    pub fn replace_external(&mut self, to_remove: UUID, to_replace_with: UUID) -> *mut u8 {
        self.storage_content[&to_replace_with] = self.storage_content[&to_remove];
        self.storage_content.remove(&to_remove);
        self.storage_content[&to_replace_with].0
    }

    pub fn move_element<T>(&mut self, element: UUID) -> T {
        let (ptr, size) = self.storage_content.remove(&element).unwrap();
        let ret: T = unsafe { std::ptr::read(ptr as *const T) };
        self.frag_hole_size += size;
        ret
    }

    fn resizeIfNeeded(&mut self, size: isize) {   
        unsafe {
            if self.storage_free_ptr.offset(size) >= self.storage.offset(self.storage_size as isize)
            || (self.frag_hole_size as f32 / self.storage_size as f32) > self.frag_threshold {
                self.resize();
            }
        }  
    }

    fn resize(&mut self) {
        self.storage_size *= 2;
        let new_storage = unsafe { realloc(self.storage, self.layout, self.storage_size) };
        let diff = (new_storage as usize) - (self.storage as usize);

        self.storage = new_storage as *mut u8;
        self.storage_free_ptr = (self.storage_free_ptr as usize + diff) as *mut u8;
        for (_, data) in self.storage_content.iter_mut() {
            data.0 = (data.0 as usize + diff) as *mut u8;
        }
    }
}

// macro_rules!  {
//     () => {
        
//     };


//     ($t1:ty, $t2:ty) => {
//         let mut container: GroupContainer = GroupContainer {
//             container
//         }
//     };
//     ($mand_1:expr, $mand_2:expr, $($opt:expr),*) => {
//         {
//             calculate! { $e }
//             calculate! { $($es),+ }
//         }
//     };
// }

pub struct UuidEntity {
    entity: UUID,
    components: Vec<UUID>
}

impl UuidEntity {
    pub fn new(ent: UUID, comps: Vec<UUID>) -> Self {
        UuidEntity {
            entity: ent,
            components: comps
        }
    }
}

struct GroupContainer<'a, 'b> {
    container: DynamicContainer,
    types: Vec<TypeId>,
    entity_size: usize,
    entities: Vec<UuidEntity>,
    component_handles: &'a HashMap<UUID, &'b dyn ComponentHandle::IComponentHandle>,
    component_sizes: &'a HashMap<TypeId, usize>
}

impl<'a, 'b> GroupContainer<'a, 'b> {
    pub fn new<'c, 'd>(
            types: Vec<TypeId>, 
            component_handles: &'c HashMap<UUID, &'d dyn ComponentHandle::IComponentHandle>,
            component_sizes: &'c HashMap<TypeId, usize>
    ) -> Self {
        let mut tot_size: usize = 0;
        for t in types {
            tot_size += component_sizes[&t];
        }

        GroupContainer { 
            container: DynamicContainer::new(1024, tot_size, 0.3),
            types: types,
            entity_size: tot_size,
            entities: vec![],
            component_handles: component_handles,
            component_sizes: component_sizes
        }
    }

    pub fn InsertEntity(&self, entity: UuidEntity) {
        self.entities.push(entity);

        let cmp: UUID = entity.components[0];
        for i in 1..self.types.len() {
            let addr = self.container.insert_external(entity.components[i], self.component_sizes[self.compo[i]);
        }

        ret
    }

    pub fn RemoveAndReplaceExternal(&self, to_remove: &UuidEntity, to_replace_with: &UuidEntity) -> *mut u8 {
        let mut returnValue = self.container.replace_external(to_remove.components[0], to_replace_with.components[0]);
        
    }
}