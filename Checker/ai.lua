--[[
	for all algorithm : 
	void aif.getTurn(game_state) : return 1 for P1 and return 2 for P2
	int aif.getNumberOfMoves(game_state) : return how many legal moves the current player have
	string aif.simulate(game_state, move_index) : return new game_state given a game_state and a move number move_index(zero-based index)
	int aif.whoWin(game_state) : return 1 for P1, return -1 for P2, return 0 for draw, and return 2 if the game is not finished yet  
	
	for all algorithm with heuristic : 
	double evaluate(game_state) : return heuristic value for given game_state, if the winner is P1, it should return Double.MAX_VALUE, and if the winner is P2, it should return Double.MIN_VALUE  
]]

if(table.unpack == nil) then table.unpack = unpack end --workaround for lua 5.2
if(unpack == nil) then unpack = table.unpack end --workaround for lua 5.1

--math.randomseed(os.time()) fixme
math.randomseed(1)
local random = math.random
local is_log = true

function log(...) 
	if(is_log) then
		print(...)
	end
end

function monteCarloCreateNode(value, parent)
	return {value = value, parent = parent, result = 0, visit = 0, childs = {}}
end

function monteCarloSelect(node, profile)			
	if(aif.whoWin(node.value) ~= 2) then -- terminal node that we have visited before, no need to expand
		return node, -1
	end
	
	local num_moves = aif.getNumberOfMoves(node.value)
	local move_index = random(num_moves)-1	
	local new_game_state = nil
	local selected_node = nil

	if(node.childs[move_index] ~= nil) then		
		--print("already exist", move_index)		
		return node.childs[move_index], move_index
	end

	new_game_state = aif.simulate(node.value, move_index)		
	
	if(profile._mc.map[new_game_state] ~= nil) then
		node.childs[move_index] = profile._mc.map[new_game_state]
		return profile._mc.map[new_game_state], move_index
	end
		
	--print("not exist yet", move_index)	
	selected_node = monteCarloCreateNode(new_game_state, node)				
	return selected_node, move_index
end

function monteCarloExpand(child, move_index, parent, profile)	
	profile._mc.map[child.value] = child
	child.parent = parent	
	parent.childs[move_index] = child	
	profile._mc.size = profile._mc.size + 1	
end

function monteCarloSimulate(node)
	local current_node = node
	local num_moves = nil
	local new_game_state = nil
	local result = aif.whoWin(current_node.value)	

	while(result == 2) do
		num_moves = aif.getNumberOfMoves(current_node.value)
		new_game_state = aif.simulate(current_node.value, random(num_moves)-1)
		current_node = monteCarloCreateNode(new_game_state, current_node)		
		result = aif.whoWin(current_node.value)		
	end	

	--print("result of simulation : ", result)
	return result
end

function monteCarloBackPropagation(node, result)
	node.result = node.result + result
	node.visit = node.visit + 1
end

function monteCarloSelectFinal(node)
	local best_move_index = nil	
	local new_game_state = nil
	local turn = aif.getTurn(node.value)
	local best_move_avg = nil
	local current_avg = nil
	
	for k, v in pairs(node.childs) do		
		current_avg = v.result / v.visit
		--print(k, turn, v.result, v.visit, current_avg)
		if ((best_move_avg == nil) or (turn == 1 and current_avg > best_move_avg) or (turn == 2 and current_avg < best_move_avg)) then
			best_move_avg = current_avg
			best_move_index = k
		end
		--print(current_avg, k, best_move_index)
	end		
	
	assert(best_move_index, "error on ai.lua : (monteCarlo) no move selected", turn, node.value, best_move_avg, current_avg)	
	return best_move_index
end

--menerima state game_state dengan jumlah kemungkinan move sebanyak num_moves dengan waktu proses maksimum sebanyak time
function monteCarlo(game_state, profile)
	profile._mc = profile._mc or {}
	profile._mc.map = profile._mc.map or {}
	profile._mc.size = profile._mc.size or 0
	local start_time = os.clock()
	local time = profile.time
	local max_tree_size = profile.max_tree_size -- fixme
	local count = 0
	local current_node = nil
	local last_node = nil
	local move_index = nil
	local root_node = nil
	if(profile._mc.map[game_state] ~= nil) then
		root_node = profile._mc.map[game_state]
	else		
		root_node = monteCarloCreateNode(game_state, nil)
		profile._mc.map[root_node.value] = root_node
	end				

	while(os.clock() - start_time < time) do		
		current_node = root_node		
		while(move_index ~= -1 and profile._mc.map[current_node.value] ~= nil) do			
			last_node = current_node
			current_node, move_index = monteCarloSelect(current_node, profile)		
		end		
		if(move_index ~= -1) then			
			monteCarloExpand(current_node, move_index, last_node, profile)
			local result = monteCarloSimulate(current_node) --simulate until terminal node
			count = count + 1
			while(current_node ~= nil) do
				monteCarloBackPropagation(current_node, result)
				current_node = current_node.parent
			end					
		end
		--print(os.clock() - start_time)
	end	
	local best_move = monteCarloSelectFinal(root_node)
	log("best_move : ", best_move, "tree size : ",profile._mc.size, "current sim count : ", count)	
	return best_move	
end

--diasumsikan player selalu jalan gantian, tidak berarti bahwa game tidak bisa membolehkan player jalan lebih dari satu
--kali berturut2, hanya saja berarti lawannya harus memilih langkah pass
function miniMax(game_state, profile)
	local depth = 1
	local node = miniMaxCreateNode(game_state)
	local value, move_index, pv = nil,nil,nil
	local start_time = os.clock()
	local elapsed_time = 0
	local best_move_index = nil
	local search_param = {}	
	search_param.fixed_depth = profile.fixed_depth -- jika parameter ini tidak nil, parameter time dihiraukan dan minimax tidak memakai iterative deepening
	search_param.max_depth = profile.max_depth -- jika paramter ini tidak nil, parameter time dihiraukan dan minimax memakai iterative deepening sampai max_depth	
	if(profile.max_depth~=nil) then 
		search_param.max_time = math.huge
	else
		search_param.max_time = profile.time or math.huge
	end
	search_param.use_tt = profile.use_tt or false -- transposition table
	search_param.no_tt_move_ordering = profile.no_tt_move_ordering or false
	search_param.start_time = start_time 
	search_param.node_visit_count = 0
	search_param.get_pv = profile.get_pv or false -- usable only if use_tt is also true
	search_param.tt = {} -- reserved untuk transposition table (very good to be used with iterative deepening) 
	
	miniMaxInitTT(search_param.tt, search_param)
	local color = aif.getTurn(game_state) == 1 and 1 or -1
	
	--fixed depth minimax
	if(search_param.fixed_depth ~= nil) then
		search_param.max_time = math.huge
		value, move_index = miniMaxRec(node, search_param.fixed_depth, color, math.huge * -1, math.huge, search_param)						
		if(search_param.get_pv) then pv = miniMaxGetPv(search_param.tt, node) end
		
		log("miniMax","value : ",value,"best_move_index : ",move_index, "depth : ", search_param.fixed_depth, "time : ", search_param.max_time, "tt count : ",search_param.tt.count, "node visited count : ",search_param.node_visit_count, "pv : ", unpack(pv or {}))
		
		return move_index
	end
	
	--iterative deepening minimax	
	repeat
		search_param.node_visit_count = 0
		value, move_index = miniMaxRec(node, depth, color, math.huge * -1, math.huge, search_param)						
		elapsed_time = os.clock() - start_time		
		if(search_param.get_pv) then pv = miniMaxGetPv(search_param.tt, node) end
		
		log("miniMax", "value : ",value, "best_move_index : ",move_index, "depth : ",depth, "time : ",search_param.max_time, "tt count : ",search_param.tt.count, "node visited count : ",search_param.node_visit_count, "pv : ", unpack(pv or {}))
		
		if(move_index >= 0) then 
			best_move_index = move_index
		end 
		if(elapsed_time > search_param.max_time / 2 or depth > 20 or (search_param.max_depth ~= nil and depth == search_param.max_depth)) then -- timeout
			return best_move_index
		end
		depth = depth + 1		
	until (elapsed_time > search_param.max_time) -- timeout
	return best_move_index
end

function miniMaxCreateNode(state)
	return {state = state}
end

--return -1 if we ran out of time (negamax)
function miniMaxRec(node, depth, color, min, max, search_param)	
	--log("visit", node.state, depth, min, max)
	
	search_param.node_visit_count = search_param.node_visit_count + 1
	local winner = aif.whoWin(node.state)
	if(winner == 1) then
		return math.huge
	elseif(winner == -1) then
		return math.huge*-1
	elseif(winner == 0 or depth == 0) then -- seri atau sudah mencapai depth paling bawah		
		return aif.evaluate(node.state) * color
	end
	
	local num_of_moves = aif.getNumberOfMoves(node.state)	
	assert(num_of_moves > 0, "this state : " .. (node.state) .. " doesnt have any moves. every node that is not a terminal node should have legal moves >= 1")
	local v_t = nil
	local best_move_index = nil		
	local move_indexes = miniMaxOrderMoves(node, true, num_of_moves, depth, search_param) -- one- based array
	
	for i=1, #move_indexes do						
		local child_node = miniMaxCreateNode(aif.simulate(node.state, move_indexes[i]))
		if(os.clock() - search_param.start_time > search_param.max_time) then return -1,-1 end -- ran out of time			
		v_t = -miniMaxRec(child_node, depth-1, color*-1, -max, -min, search_param)			
		if(v_t >= max) then 											
			if(best_move_index == nil) then best_move_index = move_indexes[i] end
			if(search_param.use_tt) then miniMaxInsertNodeTT(search_param.tt, node, aif.getTurn(node.state) == 1, max, best_move_index, depth) end
			--log("value", node.state, best_move_index, max, v_t)
			return max, best_move_index -- prune 
		end 		
		if(v_t > min) then 
			min = v_t	
			best_move_index = move_indexes[i]				
		end
	end				
	
	if(best_move_index == nil) then best_move_index = move_indexes[random(1, num_of_moves)] end -- randomize equal valued moves
	if(search_param.use_tt) then miniMaxInsertNodeTT(search_param.tt, node, aif.getTurn(node.state) == 1, min, best_move_index, depth) end
	--log("value", node.state, best_move_index)		
	return min, best_move_index
end

function miniMaxOrderMoves(cur_node, is_max, num_of_moves, depth, search_param) 
	local ret = {}
	for i=1, num_of_moves do
		ret[i] = i-1
	end 
	
	if(not search_param.no_tt_move_ordering and search_param.use_tt and search_param.tt[cur_node.state]) then
		local entry = search_param.tt[cur_node.state]
		local best_move_index = entry.best_move_index		
		ret[1] = best_move_index
		ret[best_move_index + 1] = 0		
	end	
	
	return ret
end

function miniMaxInsertNodeTT(tt, node, is_max, subtree_eval, best_move_index, depth)		
	if(tt[node.state] == nil) then -- node sudah ada
		if(tt.count > tt.max_count) then
			miniMaxRandomDeleteTT(tt)
		end
		tt[node.state] = node		
		node.is_max = is_max
		node.subtree_eval = subtree_eval
		node.depth = depth
		node.best_move_index = best_move_index								
		tt.count = tt.count + 1		
	else -- update
		local entry = tt[node.state]
		if(depth > entry.depth) then			
			entry.depth = depth
			entry.best_move_index = best_move_index
			entry.subtree_eval = subtree_eval		
		end
	end
	
end

function miniMaxRandomDeleteTT(tt)
	local sweep_count = 0
	for k,v in pairs(tt) do
		tt[k] = nil
		sweep_count = sweep_count + 1
		if(sweep_count >= tt.sweep_count) then
			break
		end
	end	
	tt.count = tt.count - tt.sweep_count
end

function miniMaxInitTT(tt, profile)
	if(not profile.use_tt) then
		tt = nil
		return
	end
	tt.count = 0
		
	tt.max_count = profile.tt_max_count or math.huge
	tt.sweep_count = profile.tt_sweep_count or 500 	
end

function miniMaxGetPv(tt, node) 
	local pv = {}

	while(node ~= nil) do
		table.insert(pv, node.best_move_index)
		local next_game_state = aif.simulate(node.state, node.best_move_index)
		node = tt[next_game_state]
	end
	
	return pv
end

local func_table = {
	monte_carlo = monteCarlo, 
	minimax = miniMax, 
}

function createProfile(profile_path, profile_name) 
	local loadedInfo = assert(loadfile(profile_path))
	local profile_table = loadedInfo()
	return profile_table[profile_name]
end

function exec(profile, game_state)
	return func_table[profile.type](game_state, profile)
end

--profile_1 as P1 and profile_2 as P2
function fight(profile_1, profile_2, game_state)
	local turn = aif.getTurn(game_state)
	--while(aif.whoWin() == ) do
	
	--end
end

