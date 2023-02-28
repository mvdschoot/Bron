use std::{
    alloc::{alloc, realloc, Layout, dealloc},
    collections::HashMap, ffi::c_void, iter::Sum,
};
use crate::{UUID::UUID, Components::component_trait};

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
        self.storage_content.insert(*uuid, (ptr, size));
        unsafe { self.storage_free_ptr = self.storage_free_ptr.offset(size as isize); }

        unsafe{ self.storage_free_ptr.sub(size) }
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


struct UuidEntity<'a> {
    entity: UUID,
    components: &'a Vec<UUID>,
}
struct GroupContainer {
    container: DynamicContainer,
    num_components: u8,
    sizes: [usize; 10]
}

impl GroupContainer {
    pub fn new(sizes: &[usize]) -> Self {
        let mut new_sizes: [usize;10];
        new_sizes.copy_from_slice(sizes);
        
        GroupContainer { 
            container: DynamicContainer::new(1024, sizes.iter().sum(), 0.3), 
            num_components: sizes.len() as u8,
            sizes: new_sizes
        }
    }

    pub fn InsertEntityExternal<'a>(&self, entity: &'a UuidEntity) -> *mut u8 {
        let cmp: UUID = entity.components[0];
        let ret: *mut u8 = self.container.insert_external(cmp, self.sizes[0]);
        for i in 1..self.num_components {
            self.container.insert_external(entity.components[i], self.sizes[i]);
        }

        ret
    }
}