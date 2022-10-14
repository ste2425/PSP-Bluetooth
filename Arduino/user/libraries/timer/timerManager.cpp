#include "timerManager.h"
#include "timer.h"

TimerManager& TimerManager::instance() {
  static TimerManager instance;

  return instance;
}

TimerManager::TimerManager() {
  last = NULL;
  first = NULL;
}

void TimerManager::update() {
  TimerNode *current = first;
  while (current != NULL) {
    current->value->update();
    current = current->next;
  }
}

void TimerManager::start() {
  TimerNode *current = first;
  while (current != NULL) {
    current->value->start();
    current = current->next;
  }
}

void TimerManager::stop() {
  TimerNode *current = first;
  while (current != NULL) {
    current->value->stop();
    current = current->next;
  }
}

void TimerManager::pause() {
  TimerNode *current = first;
  while (current != NULL) {
    current->value->pause();
    current = current->next;
  }
}

void TimerManager::reset() {
  TimerNode *current = first;
  while (current != NULL) {
    current->value->reset();
    current = current->next;
  }
}

void TimerManager::add(Timer *timer) {
  TimerNode* node = new TimerNode(timer);

  if (last == NULL) {
    last = node;
    first = node;
  } else {
    last->next = node;
    last = node;
  }
}

bool TimerManager::remove(Timer *timer) {
  TimerNode *current = first;
  TimerNode *previous = NULL;

  while (current != NULL) {
    if (current->value == timer) {
      if (previous != NULL) {
        previous->next = current->next;
      }

      if (current == last) {
        last = previous;
      }

      if (current == first) {
        first = current->next;
      }

      delete current;
      return true;
    }

    previous = current;
    current = current->next;
  }

  return false;
}
