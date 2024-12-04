#!/bin/bash

# 检查是否在 tmux 会话中
if [ -z "$TMUX" ]; then
  echo "You need to be inside a tmux session to run this script."
  echo "Please start a tmux session first."
  exit 1
fi

# 在当前 tmux 会话中创建一个新窗口
tmux new-window -n "ChatWindow" \; \
  split-window -h \; \
  split-window -v \; \
  select-pane -t 0 
  
for pane in 0 1 2; do
  tmux send-keys -t ChatWindow.$pane "./chat" C-m
done

echo "New tmux window with four panes running ./chat has been created."
