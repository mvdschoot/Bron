
use std::ptr::{copy};

pub trait IComponentHandle {
    fn move_to(&mut self, addr: *mut u8);
    fn size(&self) -> usize;
}

pub struct ComponentHandle<T> {
    component: *mut T,
}

impl<T> ComponentHandle<T> {
    pub fn from_location(loc: *mut u8) -> ComponentHandle<T> {
        ComponentHandle { component: loc as *mut T }
    }
}

impl<T> IComponentHandle for ComponentHandle<T> {
    fn move_to(&mut self, addr: *mut u8) {
        unsafe {copy(self.component as *const T, addr as *mut T, self.size())}
        self.component = addr as *mut T;
    }

    fn size(&self) -> usize {
        std::mem::size_of::<T>()
    }
}