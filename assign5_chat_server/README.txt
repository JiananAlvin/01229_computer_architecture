Assignment5 MS2
Jianan Xu
jxu147@jhu.edu

This is an individual work. Thanks to my classmates and TAs help. They gave me advice and
some ideas in this Assignment. 

Report
===================================================

NOTE： Code is shown at the end.

----Where?
We defined `members.insert(user)` and `members.erase(it)` as critical sections. Becuse we
wanna only one thread to access `members` (the set of users who have joined the room) at a
time. This makes sure that updates to `members` will never be lost, not matter how it is
accessed. For example, if two clients try enter a room at the same time, both clients must 
be registered correctly, without losing either one.

----How? Why? race conditions?
We locked a mutex before insertion and erasing operations. Thus, if a thread T is modifying 
`members`, the lock is acquired by the thread T. The other threads have to wait till the 
thread T finishes modifying `members`. After the thread finishes, the mutex will be released, 
and one of the waiting threads will be woken up and allowed to acquire it. We used `Guard` 
to guarantee mutual exclusion in criticalk sections. And `Guard` ensures we never forgett to 
release the lock. By above procesure, we avoid race conditions successfully.

----deadlocks?
As you see we, there is no circular wait or nested locks, so no deadlock happens in our program.

Similary, We also defined `m_messages.push_back(msg)` and `m_messages.pop_front()` as critical 
sections using a mutex `m_lock`. It ensures only one thread can access `m_messages` (message 
queue of a receiver) at a time. For example, if two (or more) senders send messages to a client 
at the same time, both (all) messages must be put in the message queue correctly, without losing 
either one. 

In addition, We used the semaphore `m_avail` to keep a count of how many messages are currently 
in the queue. We increment `m_avail` by calling `sem_post(&m_avail)` when a message enqueues and 
decrement it by calling `sem_timedwait(&m_avail, &ts)` when a message dequeues. The semaphore 
blocks a thread when it goes below zero. This implements the correct behaviour: if there are no 
messages available, we want the receiver to sleep until there are available messages, and each 
time a message is sent, it reduces the count of available messages by one.

Similarly, We also defined `m_rooms[room_name]` as a critical section by using `m_lock`. It means 
only one thread can access `m_rooms` (a map of room name to the pointer of corresponding room object) 
at a time. For example, if two (or more) rooms are created at the same time, both (all) 
<room name, room pointer> pairs must be put in the `m_rooms` correctly, without losing either 
one. 

----In room.cpp
----------------code------------------

void Room::add_member(User *user) {
  // - TODO: add User to the room
  if (members.count(user) <= 0) {
    Guard g(lock);
    this->members.insert(user);
  }
}

void Room::remove_member(User *user) {
  // - TODO: remove User from the room
  if (members.count(user) > 0) {
    Guard g(lock);
    members.erase(members.find(user));
  }
}

void Room::broadcast_message(const std::string &sender_username, const std::string &message_text) {
  // - TODO: send a message to every (receiver) User in the room
  Message msg;
  msg.tag = TAG_DELIVERY;
  msg.data = trim(get_room_name()) + ":" + trim(sender_username) + ":" + trim(message_text);
  Guard g(lock);
  std::set<User *>::iterator it;
  
  // Send a message to every (receiver) user's MessageQueue in the room.
  for (it = members.begin(); it != members.end(); ++it) {
    if (!((*it)->identity==User::SENDER)) {
      (*it)->mqueue.enqueue(new Message(msg.tag, msg.data));
    }
  }
}

----In message_queue.cpp
----------------code------------------

void MessageQueue::enqueue(Message *msg) {
  // - TODO: put the specified message on the queue
  Guard g(m_lock);
  m_messages.push_back(msg);
  // be sure to notify any thread waiting for a message to be
  // available by calling sem_post
  // Increments (unlocks) the semaphor.
  sem_post(&m_avail);
}

Message *MessageQueue::dequeue() {
  struct timespec ts;

  // get the current time using clock_gettime:
  // we don't check the return value because the only reason
  // this call would fail is if we specify a clock that doesn't
  // exist
  clock_gettime(CLOCK_REALTIME, &ts);

  // compute a time one second in the future
  ts.tv_sec += 1;

  // - TODO: call sem_timedwait to wait up to 1 second for a message
  //       to be available, return nullptr if no message is available
  if (sem_timedwait(&m_avail, &ts) == 0) {
    Guard g(m_lock);
    Message *msg = m_messages.front();
    // - TODO: remove the next message from the queue, return it.
    m_messages.pop_front();
    return msg;
  } else {
    return nullptr;
  }

----------------code------------------
 

----In server.cpp
----------------code------------------

Room *Server::find_or_create_room(const std::string &room_name) {
  // - TODO: return a pointer to the unique Room object representing
  //       the named chat room, creating a new one if necessary  
  // If room exists, return the room pointer.
  if (m_rooms.count(room_name) > 0) { 
    return m_rooms[room_name]; 
  } else { 
    // If room doesn't exist, create and then return the room pointer.
    Room* room = new Room(room_name);
    Guard g(m_lock);
    m_rooms[room_name] = room; 
    return room; 
  }

----------------code------------------